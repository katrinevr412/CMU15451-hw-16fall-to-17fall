#include <iostream>
#include <algorithm>
#include <stdio.h>
using namespace std;

int main() {
	int n;
	scanf("%d",&n);
	int *data = new int[n+1];
	int k = -1, p = 1;
	while(p <= n){
		k++;
		p*=2;
	}
	printf("%d\n", k);
	for(int i = 1; i <= n; i++) scanf("%d", &data[i]);
	sort(data+1, data+n+1);
	for(int i = 0; i <= k; i++){
		printf("%d", data[(1<<i)]);
		//if(i-k) printf(" ");
		//else printf("\n");
	}
	printf("\n");
	return 0;
}
