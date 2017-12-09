/*
    This code was adapted from the UBC CODEARCHIVE 2014.
    It is in the public domain.  ---Danny Sleator Nov. 2017
*/

#include <stdio.h>
#include <stdlib.h>

int allocs_in_use;
void my_free(void * p) {
    allocs_in_use--;
    free(p);
}
void * my_alloc(int size) {
    void * p = malloc(size);
    allocs_in_use++;
    if (p == NULL) {
        fprintf(stderr, "Ran out of space.  Requested size=%d.\n", size);
        exit(1);
    }
    return p;
}

/* used internally by the LP solver  */
int m, n;         /* number of constraints and variables */
int* non_basic;   /* indices of non-basic variables.  Length=n */
int* basic;       /* indices of basic variables.  Length=m */
double** a;       /* (m+1) x (n+1) tableau for simplex */

#define INF 1e100
#define EPS 1e-9

void pivot(int r, int c) {
    int i,j,t;
    t = non_basic[c];
    non_basic[c] = basic[r];
    basic[r] = t;

    a[r][c]=1/a[r][c];
    for(j=0;j<=n;j++) if(j!=c) a[r][j]*=a[r][c];
    for(i=0;i<=m;i++) if(i!=r) {
	    for(j=0;j<=n;j++) if(j!=c) a[i][j]-=a[i][c]*a[r][j];
	    a[i][c] = -a[i][c]*a[r][c];
	}
}

int feasible() {
    int r,c,i; double p,v;
    while(1) {
	for(p=INF,i=0; i<m; i++) if(a[i][n]<p) p=a[r=i][n];
	if(p>-EPS) return 1;
	for(p=0,i=0; i<n; i++) if(a[r][i]<p) p=a[r][c=i];
	if(p>-EPS) return 0;
	p = a[r][n]/a[r][c];
	for(i=r+1; i<m; i++) if(a[i][c]>EPS) {
		v = a[i][n]/a[i][c];
		if(v<p) r=i,p=v;
	    }
	pivot(r,c);
    }
}

int simplex(int m0, int n0, double* A[], double B[], double C[], double* z, double soln[]) {
    /*
    input:
      m = #constraints, n =#variables
      max C dot x s.t. A x <= B
      where A = mxn, B = m vector, C = n vector
    output:
      returns 1 (feasible), 0 (infeasible), or -1 (unbounded)
      If feasible, then stores objective value in z, and the solution in soln,
      an array of length n supplied for the variables.
    caveats:
      Cycling is possible.  Nothing is done to mitigate loss of
      precision when the number of iterations is large.
    */
    int r,c,i,j;
    double p, v;

    m = m0;
    n = n0;

    non_basic = (int *) my_alloc(n * sizeof(int));
    basic = (int *) my_alloc(m * sizeof(int));
    for(i=0; i<n; i++) non_basic[i]=i;
    for(i=0; i<m; i++) basic[i]=n+i;

    a = (double **) my_alloc((m+1) * sizeof (double *));
    
    for (i=0; i<=m; i++) {
	a[i] = (double *) my_alloc((n+1) * sizeof (double));
    }
    
    for(i=0; i<m; i++) {
	for(j=0; j<n; j++) {
	    a[i][j] = A[i][j];
	};
	a[i][n] = B[i];
    }
    for(j=0; j<n; j++) {
	a[m][j] = C[j];
    }

    a[m][n] = 0.0;
    
    if(!feasible()) return 0;

    while(1) {
	for(p=0,i=0; i<n; i++) if(a[m][i]>p) p=a[m][c=i];
	if(p<EPS) {
	    for(i=0; i<n; i++) if(non_basic[i]<n) soln[non_basic[i]]=0.0;
	    for(i=0; i<m; i++) if(basic[i]<n) soln[basic[i]]=a[i][n];
	    *z = -a[m][n];
	    return 1;
	}
	for(p=INF,i=0; i<m; i++) if(a[i][c]>EPS) {
		v = a[i][n]/a[i][c];
		if(v<p) p=v,r=i;
	    }
	if(p==INF) return -1;
	pivot(r,c);
    }
}

int main(int argc, char *argv[]) {
    double A0[] = {1.0, 3.0, 1.0};
    double A1[] = {-1.0, 0.0, 3.0};
    double A2[] = {2.0, -1.0, 2.0};
    double A3[] = {2.0, 3.0, -1.0};
    double*A[] = {A0, A1, A2, A3};

    double B[] = {3.0, 2.0, 4.0, 2.0};

    double C[] = {5.0, 5.0, 3.0};

    int m = 4;
    int n = 3;
    int i;
    double z;
    double* soln = (double *) my_alloc(n * sizeof (double));

    int ret = simplex(m, n, A, B, C, &z, soln);

    if (ret == -1) {
        printf("unbounded\n");
    } else if (ret == 0) {
        printf ("infeasible\n");	
    } else if (ret == 1) {
	printf("The optimum is: %f\n", z);
	for (i = 0; i<n; i++) {
	    printf("x%d = %f\n", i, soln[i]);
	}
    } else {
	printf("Should not have happened\n");
    }

    my_free(basic);
    my_free(non_basic);
    my_free(soln);
    for (i=0; i<=m; i++) my_free(a[i]);
    my_free(a);
    printf("allocs_in_use = %d\n", allocs_in_use);
    
    return 0;
}
