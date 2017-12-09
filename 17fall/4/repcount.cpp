#include <stdio.h>
#include <string.h>
#include <math.h>
char str[505]; //the string
int fCnt[505] = {0}; //factor count
int factors[505][100]; //the actual factor list
int MOD = 2333333; //mod used in fingerprint
int BASE = 256; //base used in fingerprint
int fingerprint[505] = {0}; //fingerprint prefix
int basepower[505]; //BASE^k's mod fingerprint
int len; //string len

typedef long long int lint;

int getDigitLength(int n) {
    //the digit length of a positive integer smaller than 1000.
    return 1 + (n > 9) + (n > 99);
}

void getFactor() {
    //compute the list of all divisors of integers from 2 to length of the string.
    for (int i = 2; i <= len; i++) {
        int smallCnt = 0, largeCnt = 0;
        int limit = (int)sqrt(i + 0.001);
        for (int j = 1; j <= limit; j++) {
            if (!(i % j)) {
                factors[i][smallCnt++] = j;
                if (j * j - i) {
                    factors[i][99 - (largeCnt++)] = i / j;
                }
            }
        }
        for (int j = 0; j < largeCnt; j++) {
            factors[i][smallCnt + j] = factors[i][100 - largeCnt + j];
        }
        fCnt[i] = smallCnt + largeCnt;
    }
}

void getFingerprints() {
    //preprocessing. compute the fingerprints for string prefixes
    fingerprint[0] = 0;
    for (int i = 1; i <= len; i++) {
        fingerprint[i] = (fingerprint[i - 1] * BASE + ((int)str[i - 1])) % MOD;
    }
    basepower[0] = 1;
    for (int i = 1; i <= len; i++) {
        basepower[i] = (basepower[i - 1] * BASE) % MOD;
    }
}

int getFingerprint(int start, int end) {
    //get the fingerprint for a specific substring.
    return (fingerprint[end + 1] + MOD - ((lint) basepower[end - start + 1] * (lint) fingerprint[start]) % MOD) % MOD;
}

bool isEqual(int start1, int start2, int compareLen) {
    //compare whether two substrings are equal using fingerprinting.
    if (getFingerprint(start1, start1 + compareLen - 1) != getFingerprint(start2, start2 + compareLen - 1)) {
        return false;
    }
    for (int i = 0; i < compareLen; i++) {
        if (str[start1 + i] != str[start2 + i]) {
            return false;
        }
    }
    return true;
}

int minLen[505][505] = {{0}}; //the min length. 0 means not computed.
int minType[505][505]; //the type that reaches the minimum.
//positive number s means it's a split, into [i, s-1] and [s, j]
//0 means using no compression is the best
//negative number -f means it's a f-fold compression

int getMinLen(int start, int end) {
    //get the min compressed length of the substring from start to end inclusive.
    if (minLen[start][end]) {
        //already computed, return the value
        return minLen[start][end];
    }
    if (!(end - start)) {
        //only one char
        minLen[start][end] = 1;
        minType[start][end] = 0;
        return 1;
    }
    //split at position from start to end-1
    int mn = 2147483647, arg = -1;
    for (int pos = start + 1; pos <= end; pos++) {
        int tmp = getMinLen(start, pos - 1) + getMinLen(pos, end);
        if (tmp < mn) {
            mn = tmp;
            arg = pos;
        }
    }
    int intvLen = end - start + 1;
    //try itself
    if (mn > intvLen) {
        mn = intvLen;
        arg = 0;
    }
    //try folds in divisors of interval length in desc order
    for (int i = fCnt[intvLen] - 1; i > 0; i--) {
        //an i-fold compression
        int fold = factors[intvLen][i];
        int foldLen = intvLen / fold;
        if (!isEqual(start, start + foldLen, intvLen - foldLen)) {
            continue;
        }
        int unitLen = getMinLen(start, start + foldLen - 1);
        int tmp = unitLen + getDigitLength(fold) + ((unitLen > 1) << 1);
        if (mn > tmp) {
            mn = tmp;
            arg = -fold;
        }
        //I'm not sure if it's OK to find one and break. Try it!
        break;
    }
    minLen[start][end] = mn;
    minType[start][end] = arg;
    return mn;
}

void printActualStr(int start, int end) {
    //prints out the actual string.
    int type = minType[start][end];
    if (!type) {
        //type 0, just output itself
        for (int i = start; i <= end; i++) {
            printf("%c", str[i]);
        }
    } else if (type > 0) {
        //positive type, meaning a split
        printActualStr(start, type - 1);
        printActualStr(type, end);
    } else {
        //negative type, meaning a direct fold
        int fold = -type;
        int foldLen = (end - start + 1) / fold;
        printf((foldLen > 1) ? "%d(" : "%d", fold);
        printActualStr(start, start + foldLen - 1);
        printf((foldLen > 1) ? ")" : "");
    }
}

int main() {
    scanf("%s", str);
    len = strlen(str);
    getFactor();
    getFingerprints();
    printf("%d\n", getMinLen(0, len-1));
    printActualStr(0, len-1);
    printf("\n");
}