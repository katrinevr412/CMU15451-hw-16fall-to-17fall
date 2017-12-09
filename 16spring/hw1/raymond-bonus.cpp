#include <bits/stdc++.h>
using namespace std;
const int MAXS = 1 << 19;
int segmin[MAXS], add[MAXS], A[200005];
void update(int sn, int s, int e, int qs, int qe, int v) {
	if (qs <= s && e <= qe) {
		segmin[sn] += v;
		add[sn] += v;
		return;
	}
	int m = (s + e) / 2, lsn = sn * 2, rsn = sn * 2 + 1;
	if (add[sn]) {
		update(lsn, s, m, s, m, add[sn]);
		update(rsn, m + 1, e, m + 1, e, add[sn]);
		add[sn] = 0;
	}
	if (qs <= m) update(lsn, s, m, qs, qe, v);
	if (qe > m) update(rsn, m + 1, e, qs, qe, v);
	segmin[sn] = min(segmin[lsn], segmin[rsn]);
}
int query(int sn, int s, int e, int qs, int qe) {
	//sn: current node number
	//s: left end, e: right end
	//qs: query left end, qe: query right end
	if (qs <= s && e <= qe) return segmin[sn];//????
	int m = (s + e) / 2, lsn = sn * 2, rsn = sn * 2 + 1, ret = INT_MAX;
	if (add[sn]) {
		update(lsn, s, m, s, m, add[sn]);
		update(rsn, m + 1, e, m + 1, e, add[sn]);
		add[sn] = 0;
	}
	if (qs <= m) ret = min(ret, query(lsn, s, m, qs, qe));
	if (qe > m) ret = min(ret, query(rsn, m + 1, e, qs, qe));
	segmin[sn] = min(segmin[lsn], segmin[rsn]);
	return ret;
}
map<int, pair<int, int> > match;
int main() {
	int T;
	scanf("%d", &T);
	while (T--) {
		int N;
		scanf("%d", &N);
		match.clear();
		for (int i = 0; i < min(MAXS, N * 4); ++i)
			segmin[i] = add[i] = 0;
		for (int i = 1; i <= N; ++i) {
			scanf("%d", &A[i]);
		}
		bool bad = false;
		for (int i = 1; i <= N; ++i) {
			int a = A[i];
			if (!match.count(a)) {//i-th does not appear before
				update(1, 1, N, 1, i, 1);
				match[a] = {1, i};
			} else {
				auto x = match[a];
				update(1, 1, N, x.first, x.second, -1);
				match[a] = {x.second + 1, i};
				update(1, 1, N, x.second + 1, i, 1);
			}
			if (!query(1, 1, N, 1, i)) {
				bad = true;
				break;
			}
		}
		printf(bad ? "boring\n" : "non-boring\n");
	}
}
