#include <stdio.h>
#include <algorithm>

struct pt{
	int x,y;
}pts[500001];

inline bool operator<(const pt& p1, const pt& p2){return p1.x<p2.x;}

int data[4000040] = {0};
int mxf[4000040] = {0};
int argmx[4000040] = {0};

inline int left(int c){return 2 * c + 1;}
inline int right(int c){return 2 * c + 2;}

void init(int start, int end, int bh){
	argmx[bh] = start;
	if(start < end) {
		int mid = (start + end) / 2;
		init(start, mid, left(bh));
		init(mid+1, end, right(bh));
	}
}

void addRange(int start, int end, int qstart, int qend, int bh, int adder){
	if(start == qstart && end == qend){
		data[bh] += adder;
	} else{
		int mid = (start + end) / 2;
		if(qend <= mid){
			addRange(start, mid, qstart, qend, left(bh), adder);
		} else if(qstart > mid){
			addRange(mid+1, end, qstart, qend, right(bh), adder);
		} else{
			addRange(start, mid, qstart, mid, left(bh), adder);
			addRange(mid+1, end, mid+1, qend, right(bh), adder);
		}
	}
	if(start == end){//leaf
		mxf[bh] = data[bh];
		argmx[bh] = start;
	} else if(mxf[left(bh)] > mxf[right(bh)]){
		mxf[bh] = data[bh] + mxf[left(bh)];
		argmx[bh] = argmx[left(bh)];
	} else{
		mxf[bh] = data[bh] + mxf[right(bh)];
		argmx[bh] = argmx[right(bh)];
	}
}

int main() {
	int s,n, mxx = 0, mxy = 0;
	scanf("%d%d",&s,&n);
	for(int i = 0; i < n; i++){
		scanf("%d%d",&pts[i].x, &pts[i].y);
		if(pts[i].x>mxx) mxx = pts[i].x;
		if(pts[i].y>mxy) mxy = pts[i].y;
	}
	std::sort(pts, pts+n);
	init(0, mxy, 0);
	int ptrAdd = 0, ptrSub = 0, mxqx = std::max(0, mxx-s), maxRes = 0, startX = 0, startY = 0;
	for(int x0 = 0; x0 <= mxqx; x0++){
		bool yj = false;
		while(ptrAdd < n && pts[ptrAdd].x <= x0 + s){
			yj = true;
			addRange(0, mxy, std::max(pts[ptrAdd].y-s, 0), pts[ptrAdd].y, 0, 1);
			ptrAdd++;
		}
		while(ptrSub < n && pts[ptrSub].x <= x0 - 1){
			addRange(0, mxy, std::max(pts[ptrSub].y-s, 0), pts[ptrSub].y, 0, -1);
			ptrSub++;
		}
		if(yj && (mxf[0] > maxRes)){
			maxRes = mxf[0];
			startX = x0;
			startY = argmx[0];
		}
		if(ptrAdd == n) break;
	}
	printf("square lower-left corner = (%d, %d)\ncount = %d\n", startX, startY, maxRes);
	return 0;
}
