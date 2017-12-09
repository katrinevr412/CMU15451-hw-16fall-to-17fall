#include <stdio.h>
#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

int row, col;
char arr[10050];
int number[10050];

void readData() {
    for (int i = 0; i < row; i++) {
        scanf("%s", arr + i * col);
    }
}

const int UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3;
int dirs[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
bool inBound(int r, int c) {
    return r >= 0 && r < row && c >= 0 && c < col;
}

int opposite(int dir) {
    //get the opposite direction
    return (dir + 2) % 4;
}

struct node {
    int level; //the level
    int number; //the numbers
    node *fwdList[4]; //the fwd nodes. C -> A -> T -> S. if not valid then NULL
    node *bckList[4]; //the bck nodes. S -> T -> A -> C. if not valid then NULL
    bool fwdTag[4]; //whether the fwd edge is reversed. initialized to false
    bool bckTag[4]; //whether the bck edge is reversed. initialized to false
    bool sVisited; //the visited flag on source side
    bool tVisited; //the visited flag on sink side
    bool used; //whether this node has been used
    node *prev;
    int trc;
    void init(int level, int number) {
        this -> level = level;
        this -> number = number;
        used = false;
        for (int i = 0; i < 4; i++) {
            fwdList[i] = NULL;
            bckList[i] = NULL;
            fwdTag[i] = false;
            bckTag[i] = false;
        }
    }
} nodes[4][10000];

//number of nodes in a given level
int numNodesInLevel[4] = {0,0,0,0};

int getLevelNum(char c) {
    switch (c) {
      case 'c': return 0;
      case 'a': return 1;
      case 't': return 2;
      case 's': return 3;
      default: return -1;
    }
}

void getNumber() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int level = getLevelNum(arr[i * col + j]);
            if (level != -1) {
                number[i * col + j] = numNodesInLevel[level]++;
            }
        }
    }
}

void buildGraph() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int level = getLevelNum(arr[i * col + j]);
            if (level == -1) {
                continue;
            }
            int numb = number[i * col + j];
            node *thisNode = &nodes[level][numb];
            thisNode -> init(level, numb);
            for (int dir = 0; dir < 4; dir++) {
                int ni = i + dirs[dir][0];
                int nj = j + dirs[dir][1];
                if (inBound(ni, nj)) {
                    int nlevel = getLevelNum(arr[ni * col + nj]);
                    if (nlevel == level + 1) {
                        //in fwd list
                        thisNode -> fwdList[dir] = &nodes[nlevel][number[ni * col + nj]];
                    } else if (nlevel == level - 1 && nlevel != -1) {
                        //in bck list
                        thisNode -> bckList[dir] = &nodes[nlevel][number[ni * col + nj]];
                    }
                }
            }
        }
    }
}

void assertIsFlow() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < numNodesInLevel[i]; j++) {
            node *curNode = &nodes[i][j];
            assert (curNode -> level == i);
            if (i < 3) {
                for (int dir = 0; dir < 4; dir++) {
                    if (curNode -> fwdList[dir]) {
                        node *nbNode = curNode -> fwdList[dir];
                        assert (nbNode -> level == i + 1);
                        assert (curNode -> fwdTag[dir] == nbNode -> bckTag[opposite(dir)]);
                    }
                }
            }
            if (i > 0) {
                for (int dir = 0; dir < 4; dir++) {
                    if (curNode -> bckList[dir]) {
                        node *nbNode = curNode -> bckList[dir];
                        assert (nbNode -> level == i - 1);
                        assert (curNode -> bckTag[dir] == nbNode -> fwdTag[opposite(dir)]);
                    }
                }
            }
        }
    }
}

bool dfs(node *n, bool side) {
    //side: false means on src side, true on snk side
    //first marked as visited
    if (!side) {
        n -> sVisited = true;
    } else {
        n -> tVisited = true;
    }
    //case 1: if in level 3, check if its fwdTag[0] is set. If not, set it and return true
    if (n -> level == 3 && side && !(n -> fwdTag[0])) {
        n -> fwdTag[0] = true;
        return true;
    }
    if (!side) {
        //if on src side
        if (!n -> used && !n -> tVisited && dfs(n, true)) {
            n -> used = true;
            return true;
        }
        if (n -> level > 0) {
            //find a bck edge
            for (int dir = 0; dir < 4; dir++) {
                if (n -> bckList[dir] != NULL && n -> bckTag[dir] && !(n -> bckList[dir] -> tVisited) && dfs(n -> bckList[dir], true)) {
                    n -> bckTag[dir] = false;
                    n -> bckList[dir] -> fwdTag[opposite(dir)] = false;
                    return true;
                }
            }
        }
    } else {
        //if on snk side
        if (n -> used && !n -> sVisited && dfs(n, false)) {
            n -> used = false;
            return true;
        }
        if (n -> level < 3) {
            //find a fwd edge
            for (int dir = 0; dir < 4; dir++) {
                if (n -> fwdList[dir] != NULL && !(n -> fwdTag[dir]) && !(n -> fwdList[dir] -> sVisited) && dfs(n -> fwdList[dir], false)) {
                    //set its reverse tag, and its successor's reverse tag
                    n -> fwdTag[dir] = true;
                    n -> fwdList[dir] -> bckTag[opposite(dir)] = true;
                    return true;
                }
            }
        }
    }
    return false;
}

bool findPath(int &res) {
    //find an augmenting path and increment res. if no more path, return false
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < numNodesInLevel[i]; j++) {
            nodes[i][j].sVisited = false;
            nodes[i][j].tVisited = false;
        }
    }
    for (int i = 0; i < numNodesInLevel[0]; i++) {
        if (nodes[0][i].bckTag[0]) {
            //already backed, ignore
            continue;
        }
        if (dfs(&nodes[0][i], false)) {
            res++;
            nodes[0][i].bckTag[0] = true; //set to be occupied
            return true;
        }
    }
    return false;
}

int solve() {
    int res = 0;
    while (findPath(res)) {
        ;
    }
    return res;
}

char cats[5] = "cats";

void output(int result) {
    if (!(result - 1)) {
        cats[3] = '\0';
    }
    printf("%d %s\n", result, cats);
}

int main() {
    scanf("%d%d", &row, &col);
    readData();
    getNumber();
    buildGraph();
    output(solve());
    return 0;
}