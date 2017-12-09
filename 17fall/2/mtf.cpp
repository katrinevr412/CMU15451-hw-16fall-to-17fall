#include <stdio.h>
#include <algorithm>
int segTree[8000008] = {0}, idx[1000010], n, m, val;
int get(int qStart, int qEnd, int start, int end, int nodeNo) {
    if (qStart == start && qEnd == end) {
        return segTree[nodeNo];
    }
    int mid = (start + end) / 2;
    if (qEnd <= mid) {
        return get(qStart, qEnd, start, mid, (nodeNo << 1) + 1);
    } else if (qStart > mid) {
        return get(qStart, qEnd, mid + 1, end, (nodeNo << 1) + 2);
    } else {
        return get(qStart, mid, start, mid, (nodeNo << 1) + 1) + get(mid + 1, qEnd, mid + 1, end, (nodeNo << 1) + 2);
    }
}
void update(int idx, int start, int end, int nodeNo, int value) {
    int mid = (start + end) / 2;
    if (start < end && idx <= mid) {
        update(idx, start, mid, (nodeNo << 1) + 1, value);
    } else if (start < end) {
        update(idx, mid + 1, end, (nodeNo << 1) + 2, value);
    }
    segTree[nodeNo] += value;
}
int main() {
    scanf("%d%d", &n, &m);
    for (int i = 0; i < n; i++) {
        idx[i] = i + m;
    }
    for (int i = 0; i < m; i++) {
        scanf("%d", &val);
        int ix = idx[val];
        printf("%d ", get(0, ix, 0, n + m - 1, 0) + std::max(0, ix - m + 1) - 1);
        idx[val] = m - 1 - i;
        update(ix, 0, n + m - 1, 0, -1);
        update(m - 1 - i, 0, n + m - 1, 0, 1);
    }
}