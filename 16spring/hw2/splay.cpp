#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstring>
#include <fstream>
#include <string>
using namespace std;

const int ZERO = -1;

class node{
public:
	int data;
	node *parent;
	node *left, *right;
	node():data(ZERO),parent(NULL), left(NULL), right(NULL){}
};

node *nodePool, *root;
bool printTrc;

int sum = 0;

void buildTree(int n){
	nodePool = new node[n];
	for(int i = 0; i < n; i++){
		nodePool[i].data = i;
		nodePool[i].parent = NULL;
		nodePool[i].left = NULL;
		nodePool[i].right = NULL;
	}
	root = &nodePool[n-1];
	for(int i = n-1; i > 0; i--){
		nodePool[i].left = &nodePool[i-1];
		nodePool[i-1].parent = &nodePool[i];
	}
	root = &nodePool[n-1];
}

void printTree(node *r){
	if(!r) {
		cout << "()" ;
		return;
	}

	if(r->left && r->left->parent != r){
		cout << "***ERROR: " << r->left->data << "'s parent is not " << r->data << "***";
	}
	if(r->right && r->right->parent != r){
		cout << "***ERROR: " << r->right->data << "'s parent is not " << r->data << "***";
	}

	cout << "(" << r->data << " ";
	printTree(r->left);
	printTree(r->right);
	cout << ")";
}

node *find(int label, ostream& o){
	//return pointer to the node found.
	//if not found return NULL(never occurs)
	//update the depth into sum
	int depth = 0;
	node *iter = root;
	while(iter->data != label){
		depth++;
		if(!iter->left && label < iter->data) return NULL;
		if(!iter->right && label > iter->data) return NULL;
		if(label < iter->data){
			iter = iter->left;
		}
		else{
			iter = iter->right;
		}
	}
	sum += depth;
	//cout << "depth: " << depth << endl;
	if(printTrc) o << depth << " ";
	return iter;
}

void splay(node *splayNode){
	//splay a node.
	if(!splayNode){
		cout << "ERROR" << endl;
		return;
	}
	node *cur = splayNode, *pred;
	while((pred = cur->parent) != NULL){
		//printTree(root);
		//cout << endl;
		if(pred == root){
			//case Zig
			if(cur == pred->left){
				node *a = cur->left, *b = cur->right, *c = pred->right;
				cur->left = a, cur->right = pred, pred->left = b, pred->right = c;
				cur->parent = NULL, pred->parent = cur;
				if(a) a->parent = cur;
				if(b) b->parent = pred;
				if(c) c->parent = pred;
			}
			else{
				node *a = cur->right, *b = cur->left, *c = pred->left;
				cur->right = a, cur->left = pred, pred->right = b, pred->left = c;
				cur->parent = NULL, pred->parent = cur;
				if(a) a->parent = cur;
				if(b) b->parent = pred;
				if(c) c->parent = pred;
			}
			root = cur;
			return;
		}
		node *z = pred->parent;
		node *zParent = z->parent;
		bool left;
		if(zParent){
			//record left-right-ness of this subtree from z
			left = (z==zParent->left);
			//cout << cur->data << " " << pred->data << " " << z->data << " " << zParent->data<< endl;
		}
		if(pred == z->left && cur == pred->right){
			//case Zig-zag.1
			node *a = pred->left, *b = cur->left, *c = cur->right, *d = z->right;
			cur->left = pred, cur->right = z;
			pred->left = a, pred->right = b;
			z->left = c, z->right = d;
			if(a) a->parent = pred;
			if(b) b->parent = pred;
			if(c) c->parent = z;
			if(d) d->parent = z;
			pred->parent = cur;
			z->parent = cur;
		}
		if(pred == z->right && cur == pred->left){
			//case Zig-zag.2
			node *a = pred->right, *b = cur->right, *c = cur->left, *d = z->left;
			cur->right = pred, cur->left = z;
			pred->right = a, pred->left = b;
			z->right = c, z->left = d;
			if(a) a->parent = pred;
			if(b) b->parent = pred;
			if(c) c->parent = z;
			if(d) d->parent = z;
			pred->parent = cur;
			z->parent = cur;
		}
		if(pred == z->left && cur == pred->left){
			//case Zig-zig.1
			node *a = cur->left, *b = cur->right, *c = pred->right, *d = z->right;
			cur->left = a, cur->right = pred;
			pred->left = b, pred->right = z;
			z->left = c, z->right = d;
			if(a) a->parent = cur;
			if(b) b->parent = pred;
			if(c) c->parent = z;
			if(d) d->parent = z;
			z->parent = pred;
			pred->parent = cur;
		}
		if(pred == z->right && cur == pred->right){
			//case Zig-zig.2
			node *a = cur->right, *b = cur->left, *c = pred->left, *d = z->left;
			cur->right = a, cur->left = pred;
			pred->right = b, pred->left = z;
			z->right = c, z->left = d;
			if(a) a->parent = cur;
			if(b) b->parent = pred;
			if(c) c->parent = z;
			if(d) d->parent = z;
			z->parent = pred;
			pred->parent = cur;
		}
		if(!zParent){
			//cout << "BOOM" << endl;
			//cout << cur->data << endl;
			cur->parent = NULL;
			root = cur;
			return;
		}
		cur->parent = zParent;
		if(left) zParent->left = cur;
		else zParent->right = cur;
	}
}

void generateRandom(int *splayList, int n, int m){
	for(int i = 0; i < m; i++){
		//cout << "random number is: " << (splayList[i] = rand() % n) << endl;
		splayList[i] = rand() % n;
	}
}

int main(int argc, char **argv) {
	if(argc == 1){
		printTrc = true;
		int n,m;
		cin >> n >> m;
		buildTree(n);
		for(int i = 0; i < m; i++){
			int tmp;
			cin >> tmp;
			//printTree(root);
			//cout << endl;
			splay(find(tmp, cout));
		}
		//cout << sum * 1.0 / m << endl;
		printf("\n%.3lf\n", sum*1.0/m);
		delete [] nodePool;
	}
	else if(!strcmp("-t", argv[1])){
		printTrc = false;
		srand (time(NULL));
		//have arguments, time test mode
		for(int i = 2; i+1 < argc; i+=2){
			int n = atoi(argv[i]), m = atoi(argv[i+1]);
			if(n <= 0 || m < 0) continue;
			sum = 0;
			int *splayList = new int[m];
			clock_t start = clock();
			buildTree(n);
			generateRandom(splayList, n, m);
			for(int j = 0; j < m; j++){
				splay(find(splayList[j], cout));
			}
			printf("\n%.3lf\n", sum*1.0/m);
			delete [] nodePool;
			clock_t end = clock();
			printf("for %d elements and %d splays, running time is %.3lfms.\n", n, m, (end-start)*1000.0/CLOCKS_PER_SEC);
			delete [] splayList;
		}
	}
	else if(!strcmp("-c", argv[1])){
		printTrc = true;
		srand (time(NULL));
		int caseNo = atoi(argv[2]);
		for(int i = 3; i+1 < argc; i+=2){
			int n = atoi(argv[i]), m = atoi(argv[i+1]);
			if(n <= 0 || m < 0) continue;
			char inputStr[32];
			sprintf(inputStr, "%d.in", caseNo);
			ofstream inputFile(inputStr);
			inputFile << n << " " << m << endl;
			int *splayList = new int[m];
			buildTree(n);
			generateRandom(splayList, n, m);
			for(int j = 0; j < m; j++){
				inputFile << splayList[j] << " ";
			}
			inputFile << endl;
			inputFile.close();
			char outputStr[32];
			sprintf(outputStr, "%d.out", caseNo);
			ofstream outputFile(outputStr);
			for(int j = 0; j < m; j++){
				splay(find(splayList[j], outputFile));
			}
			outputFile << endl;
			char buf[256];
			sprintf(buf, "%.3lf", sum*1.0/m);
			string tmpStr(buf);
			outputFile << tmpStr << endl;
			outputFile.close();
			caseNo++;
			delete [] nodePool;
			delete [] splayList;
		}
	}
	return 0;
}
