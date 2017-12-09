#include <stdio.h>
#include <cmath>
#include <algorithm>
using namespace std;
int n;
double r;
const double TWOPI = 2 * 3.141592654;
const int SIZE = 1 << 11;
//store all the points
struct point {
    double x,y; point(double x, double y): x(x), y(y){} point(): x(-1.), y(-1.){}
} points[SIZE];

typedef point interval;

//events, in angle value from 0 to 2pi
double addEvents[SIZE], subEvents[SIZE];

inline double round(double angle) {
    //round an angle into [0, 2pi)
    return angle + TWOPI * (angle < 0.) - TWOPI * (angle >= TWOPI);
}

inline double dist(point *a, point *b) {
    return sqrt((a -> x - b -> x) * (a -> x - b -> x) + (a -> y - b -> y) * (a -> y - b -> y));
}

interval getInterval(point *from, point *to) {
    //return the angle interval that can be the ray from FROM to circumcentre
    //such that TO is inside the circle
    double cenAngle = atan2(to -> y - from -> y, to -> x - from -> x), distft;
    double flucAngle = ((distft = dist(from, to)) > 2 * r) ? -1. : acos(dist(from, to) / 2 / r);
    return (flucAngle < -.5) ? interval() : interval(round(cenAngle - flucAngle), round(cenAngle + flucAngle));
}

void updateCnt(int *cnt, int newCnt) {
    *cnt = (*cnt > newCnt) ? *cnt : newCnt;
}

int main() {
    scanf("%d%lf", &n, &r);
    r += 1e-6;
    for (int i = 0; i < n; i++) {
        scanf("%lf%lf", &points[i].x, &points[i].y);
    }
    int cnt = 0;
    for (int i = 0; i < n; i++) {//i is the index of FROM points we consider.
        int zeroCnt = 0; //count the initial value.
        int addCnt = 0, subCnt = 0;
        for (int j = 0; j < n; j++) {//j is the index of TO points we consider.
            if (!(i - j)) {
                continue;
            }
            interval intv = getInterval(points + i, points + j);
            if (intv.x < -.5) {
                continue;
            }
            if (intv.x > intv.y) {
                //this means the interval passes zero.
                zeroCnt++;
            }
            addEvents[addCnt++] = intv.x;
            subEvents[subCnt++] = intv.y;
        }
        sort(addEvents, addEvents + addCnt);
        sort(subEvents, subEvents + subCnt);
        //update cnt
        updateCnt(&cnt, zeroCnt);
        int curAdd = 0, curSub = 0;
        while (1) {
            if (!(curAdd - addCnt)) {
                //we've gone through all ADD events, done
                break;
            } else if (!(curSub - subCnt)) {
                //there is no SUB event to compare, add all the remaining, update and break
                updateCnt(&cnt, zeroCnt + addCnt - curAdd);
                break;
            } else {
                //compare the timeline of two events.
                if (addEvents[curAdd] <= subEvents[curSub]) {
                    //perform the ADD event, and update. Note that ADD is always prioritized if there is a tie.
                    updateCnt(&cnt, ++zeroCnt);
                    curAdd++;
                } else {
                    //perform the SUB event, no need to update
                    zeroCnt--;
                    curSub++;
                }
            }
        }
    }
    printf("%d\n", cnt + 1);
}