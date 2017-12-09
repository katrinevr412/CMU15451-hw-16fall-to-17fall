#include <stdio.h>
#include <string.h>
char str[30003];
int main() {
    scanf("%s", str);
    int len = strlen(str);
    for (int t = len / 2; t >= 1; t--) {
        int a = 0, b = t, lim = len - 2 * t, covered = 0;
        while (a - covered <= lim) {
            if (t % 2 == 1 && covered == (t - 1) / 2) {
                if (str[a] == str[b]) {
                    printf("%d %d\n", a - covered, t);
                    return 0;
                } else {
                    covered = 0;
                    a++, b++;
                }
            } else {
                int offset = (t + 1) / 2;
                if (str[a + offset] != str[b + offset]) {
                    covered = 0;
                    a += offset+1;
                    b += offset+1;
                } else if (str[a] != str[b]) {
                    covered = 0;
                    a++, b++;
                } else {
                    covered++, a++, b++;
                    if (covered == (t + 1) / 2) {
                        printf("%d %d\n", a - covered, t);
                        return 0;
                    }
                }
            }
        }
    }
    printf("-1\n");
}