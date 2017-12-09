#include <iostream>
#include <stdio.h>
#include <stack>
using namespace std;

const int mxSt = 1 << 20;
const int mxSz = 20;

int dijDist[mxSz][mxSz];
int dijTrc[mxSz][mxSz];
int tempDist[mxSz][mxSz][2];
int tempTrc[mxSz][mxSz][2];
int tspDist[mxSz][mxSt];
int tspTrc[mxSz][mxSt];

int n;
const int MX = (int) 1e9;
const int ZERO = 0;

inline int mn(int a, int b){
	return a>b?b:a;
}

inline bool have(int state, int pos){
	//test whether state contains a certain position
	return (state & (1 << pos));
}

inline int exclude(int state, int pos){
	//exclude a specific position from state
	return (state ^ (1 << pos));
}

void init(){
	int m;
	scanf("%d%d", &n, &m);
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(i == j) {
				tempDist[i][j][0] = 0;//zero distance to itself
				dijDist[i][j] = 0;
			}
			else {
				tempDist[i][j][0] = MX;//set to infty
				dijDist[i][j] = MX;
			}
		}
	}
	int i,j,dij,dji;
	while(m--){
		scanf("%d%d%d%d", &i,&j,&dij,&dji);
		if(i==j) continue;//ignore self loop
		if(dij < tempDist[i][j][0]){
			tempDist[i][j][0] = dij;
			dijDist[i][j] = dij;
			tempTrc[i][j][0] = i;
		}
		if(dji < tempDist[j][i][0]){
			tempDist[j][i][0] = dji;
			tempTrc[j][i][0] = j;
		}
	}
}

void fw(){
	//use floyd-warshall to compute distance (stored in dijDist), and place precursors into dijTrc
	for(int k = 0; k < n; k++){
		int from, to;//src and dest temp array index
		if(k%2==0){
			from = 0, to = 1;
		}
		else{
			from = 1, to = 0;
		}
		for(int i = 0; i < n; i++){
			for(int j = 0; j < n; j++){
				if(i==j) continue;//assume no negative weights
				int newDist = tempDist[i][k][from] + tempDist[k][j][from];
				if(newDist < tempDist[i][j][from]){
					tempDist[i][j][to] = newDist;
					tempTrc[i][j][to] = tempTrc[k][j][from];
				}
				else{
					tempDist[i][j][to] = tempDist[i][j][from];
					tempTrc[i][j][to] = tempTrc[i][j][from];
				}
			}
		}
	}
	int tar = n%2;//target index
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			dijDist[i][j] = tempDist[i][j][tar];
			dijTrc[i][j] = tempTrc[i][j][tar];
			//cout << "from " << i << " to " << j << ": " << dijDist[i][j] << ", last place to go: " << dijTrc[i][j] << endl;
		}
	}
}

void tsp(){
	tspDist[0][1] = 0; //initial state

	int stUb = 1 << n;

	for(int state = 3; state < stUb; state += 2){
		tspDist[0][state] = MX;
	}

	for(int state = 3; state < stUb; state += 2){//because we assume starting at 0, state is always an odd number
		for(int i = 1; i < n; i++){
			if(!have(state, i)) continue;
			int last = exclude(state, i);
			int thisMin = MX, arg = -1;
			for(int j = 0; j < n; j++){
				if(i == j || !have(state, j)) continue;
				int newMin = tspDist[j][last] + dijDist[j][i];
				if(newMin < thisMin){
					thisMin = newMin;
					arg = j;
				}
			}
			tspDist[i][state] = thisMin;
			tspTrc[i][state] = arg;
			//cout << "trc" << i << "," << state << ":" << tspTrc[i][state] << endl;
		}
	}
}

void pushPath(int from, int to, stack<int> &trc){
	int cur = to;
	while(cur != from){
		int last = dijTrc[from][cur];
		trc.push(last);
		cur = last;
	}
}

void printTrc(){
	int tar = (1 << n) - 1;
	//cout << tar << endl;
	int minDist = MX, arg = -1;
	for(int i = 1; i < n; i++){
		int thisDist = dijDist[i][0] + tspDist[i][tar];
		if(thisDist < minDist) {
			minDist = thisDist;
			arg = i;
		}
	}
	//cout << arg << endl;
	printf("optimal tour cost = %d\ntour:", minDist);
	stack<int> trc;
	trc.push(0);
	//trc.push(arg);
	int rubb = 0;
	while(1){
		rubb = dijTrc[arg][rubb];
		trc.push(rubb);
		if(rubb == arg) break;
	}
	int cur = arg;
	int state = tar;
	while(1){
		//cout << state << endl;
		int last = tspTrc[cur][state];
		//cout << last << endl;
		state = exclude(state, cur);
		//cout << state << endl;
		//push the vertexes from cur to last to the stack
		pushPath(last, cur, trc);
		cur = last;
		if(!cur) break;
	}
	while(!trc.empty()){
		printf(" %d", trc.top());
		trc.pop();
	}
	printf("\n");
}


int main() {
	//initialize
	init();
	if(n == 1){
		printf("optimal tour cost = 0\ntour: 0\n");
		return 0;
	}
	//floyd-warshall
	fw();
	//tsp
	tsp();
	//trace back
	printTrc();
	return 0;
}
