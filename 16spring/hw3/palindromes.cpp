#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <stdlib.h>
#include <cmath>
#include <assert.h>
#include <stdio.h>
#include <sstream>
using namespace std;

typedef long long int lint;

lint LOWER_P = 1000000000;
lint UPPER_P = 2000000000;
const lint PRIME_THRES = 23333;
const lint PRIME_FAILURE = -1;
const lint BASE = 256;

lint *hashVal, *revHashVal, *baseVal;
lint prime, strLen;

bool isPrime(lint p){
	if(!(p%2)) return false;//we cannot have p=2!
	lint sqt = (lint)(sqrt(p+0.1));
	for(int i = 3; i <= sqt; i += 2){
		if(!(p%i)) return false;
	}
	return true;
}

lint getRandomPrime(){
	int cnt = 0;
	while(cnt < PRIME_THRES){
		lint rnd = (lint)(LOWER_P + 2 * (rand() % (UPPER_P/2 - LOWER_P/2)) + 1);
		if(isPrime(rnd)){
			return rnd;
		}
		cnt++;
	}
	return PRIME_FAILURE;
}

lint hV(int i, int j){
	//find the hash value from i to j inclusive, O(1)
	if(j == i-1) return 0;
	assert(i <= j);
	return (hashVal[j+1] + prime - (hashVal[i] * baseVal[j+1-i]) % prime) % prime;
}

lint rhV(int i, int j){
	//find the reverse hash value from i to j inclusive, O(1)
	if(j == i-1) return 0;
	assert(i <= j);
	return (revHashVal[i] + prime - (revHashVal[j+1] * baseVal[j+1-i]) % prime) % prime;
}

int test(string &s, int l){
	//test whether length l palindrome works
	for(int i = 0; i <= strLen - l; i++){
		int j = i + l - 1;
		if(hV(i,j) == rhV(i,j)) return i;
	}
	return -1;
}

void oddBinSrch(string &s, int *low, int *high, int *curStart){
	//from 2*low+1 to 2*high+1
	//returns new low and high values, store start place at curStart
	int mid = (*low + *high + 1) / 2, temp;
	if((temp = test(s, 2*mid+1)) != -1){
		*low = mid;
		*curStart = temp;
	}
	else{
		*high = mid-1;
	}
}

void evenBinSrch(string &s, int *low, int *high, int *curStart){
	//from 2*low to 2*high
	int mid = (*low + *high + 1) / 2, temp;
	if((temp = test(s, 2*mid)) != -1){
		*low = mid;
		*curStart = temp;
	}
	else{
		*high = mid - 1;
	}
}

void findMaxPalindrome(string &s, lint *Start, lint *Len){
	//find the longest palindrome in s.
	//put the result in *Start and *Len
	strLen = (lint)s.length();
	prime = getRandomPrime();
	//validate the prime
	assert(prime != PRIME_FAILURE);
	cout << "trying modulus " << prime << endl;

	//hash value of powers of BASE
	baseVal = new lint[strLen+1];
	baseVal[0] = 1;
	for(int i = 0; i < strLen; i++){
		baseVal[i+1] = (baseVal[i] * BASE) % prime;
	}

	hashVal = new lint[strLen+1];
	revHashVal = new lint[strLen+1];
	//hash value of prefixes
	hashVal[0] = 0;
	for(int i = 0; i < strLen; i++){
		hashVal[i+1] = (s[i] + hashVal[i] * BASE) % prime;
	}
	//hash value of suffixes
	revHashVal[strLen] = 0;
	for(int i = strLen-1; i >= 0; i--){
		revHashVal[i] = (revHashVal[i+1] * BASE + s[i]) % prime;
	}

	int oddLow = 0, evenLow = 0;
	int oddHigh = (strLen - 1) / 2, evenHigh = strLen / 2;
	int curOddStart = 0, curEvenStart = 0;

	while(oddLow < oddHigh){
		oddBinSrch(s, &oddLow, &oddHigh, &curOddStart);
	}

	while(evenLow < evenHigh){
		evenBinSrch(s, &evenLow, &evenHigh, &curEvenStart);
	}

	int oddRes = 2 * oddLow + 1, evenRes = 2 * evenLow;
	if(oddRes > evenRes){
		*Start = (lint)curOddStart;
		*Len = (lint)oddRes;
	}
	else{
		*Start = (lint)curEvenStart;
		*Len = (lint)evenRes;
	}
	//free space
	delete [] baseVal;
	delete [] hashVal;
	delete [] revHashVal;
}

bool isPalindrome(string &s, lint pos, lint len){
	for(int i = pos; i < pos + len/2; i++){
		if(s[i] != s[2*pos+len-1-i]) return false;
	}
	return true;
}

void print(string &s, lint pos, lint len){
	for(int i = pos; i < pos+len; i++){
		cout << s[i];
	}
	cout << endl;
}

int main(int argc, char **argv) {
	//bool ignore = false;
	srand(time(NULL));
	string s;
	getline(cin, s);
	int B;
	stringstream ss;
	ss << s;
	ss >> B;
	getline(cin, s);
	//if(!ignore){
	LOWER_P = B;
	UPPER_P = 2 * B;
	if(B%2) LOWER_P--;
	//}
	//time_t start, end;
	//start = clock();
	while(1){
		lint startPos, palinLen;
		findMaxPalindrome(s, &startPos, &palinLen);
		//cout << startPos << " " << palinLen << endl;
		if(isPalindrome(s, startPos, palinLen)){
			//print(s, startPos, palinLen);
			cout << "start=" << startPos << " length=" << palinLen << endl;
			break;
		}

	}
	//end = clock();
	//printf("With string of length %d, running time is %.3lfs.\n", s.length(), (end-start)*1.0/CLOCKS_PER_SEC);

	return 0;
}
