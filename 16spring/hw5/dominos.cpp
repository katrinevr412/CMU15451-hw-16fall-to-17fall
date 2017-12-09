#include <iostream>
#include <stdio.h>
#include <queue>
using namespace std;

int r,c;
char map[32][32];
int bh[32][32];
int odd = 0, even = 0;
int oddmatch[1000] = {0}, evenmatch[1000] = {0};
int evenadjNo[1000] = {0}, oddadjNo[1000] = {0};
int evenadjList[1000][4], oddadjList[1000][4];
int res = 0;

void printbh(){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < c; j++){
			cout << bh[i][j] << " ";
		}
	}
}

void getbh(){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < c; j++){
			if(map[i][j] == 'x'){
				bh[i][j] = -1;
			}
			else if((i+j)%2==0){
				bh[i][j] = (++even);
			}
			else{
				bh[i][j] = (++odd);
			}
		}
	}
}

bool inbound(int rr, int cc){
	return rr>=0 && rr<r && cc>=0 && cc<c;
}

void getgraph(){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < c; j++){
			if(bh[i][j] == -1 || (i+j)%2==1) continue;
			int dir[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
			for(int k = 0; k < 4; k++){
				int x = i+dir[k][0], y = j+dir[k][1];
				if(inbound(x,y) && bh[x][y] != -1){
					int bhou = bh[i][j], bhji = bh[x][y];
					oddadjList[bhji][oddadjNo[bhji]++] = bhou;
					evenadjList[bhou][evenadjNo[bhou]++] = bhji;
				}
			}
		}
	}
}


void flow(){
	while(1){
		queue<int> bfs;
		int lastOdd = -1;
		int trc[1000];
		bool oddused[1000] = {0};
		bool evenused[1000] = {0};
		for(int i = 1; i <= even; i++){
			if(!evenmatch[i]){
				bfs.push(i);
				evenused[i] = 1;
			}
		}
		while(!bfs.empty()){
			int t = bfs.front();
			bfs.pop();
			if(t > 0){
				//on the even side
				for(int i = 0; i < evenadjNo[t]; i++){
					int nextodd = evenadjList[t][i];
					if(!oddused[nextodd]){
						oddused[nextodd] = 1;
						bfs.push(-nextodd);
						trc[nextodd] = t;
						if(!oddmatch[nextodd]){
							lastOdd = nextodd;
							goto done;
						}
					}
				}
			}
			else{
				//on the odd side
				t = -t;
				if(!oddmatch[t]){
					lastOdd = t;
					goto done;
				}
				if(!evenused[oddmatch[t]]){
					bfs.push(oddmatch[t]);
					evenused[oddmatch[t]] = 1;
				}
			}
		}
		done:
		if(lastOdd==-1) return;
		res++;
		int lastEven = trc[lastOdd];
		while(1){
			int oldOdd = evenmatch[lastEven];
			evenmatch[lastEven] = lastOdd;
			oddmatch[lastOdd] = lastEven;
			if(!oldOdd) break;
			lastOdd = oldOdd;
			lastEven = trc[lastOdd];
		}
	}
}

void printres(){
	printf("%d domino", res);
	if(res-1) printf("s");
	printf("\n");
}

int main() {
	scanf("%d%d",&r,&c);
	for(int i = 0; i < r; i++){
		scanf("%s", map[i]);
	}
	getbh();
	getgraph();
	flow();
	printres();
	return 0;
}
