#include <stdio.h>
int n, tmp, arr[1000100], rcd[2000200], cnt = 0;
int main() {
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", arr+i);
    for (int i = 0; i < n; i++) {
        while ((tmp = arr[i]) != i) {
            arr[i] = arr[tmp];
            arr[tmp] = tmp;
            rcd[cnt << 1] = i;
            rcd[((cnt++) << 1) + 1] = tmp;
        }
    }
    printf("%d\n", cnt);
    for (int i = 0; i < cnt; i++) printf("%d %d\n", rcd[i << 1], rcd[(i << 1) + 1]);
    return 0;
}