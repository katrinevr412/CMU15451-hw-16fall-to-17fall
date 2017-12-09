//    This code was adapted from the UBC CODEARCHIVE 2014.
//    It is in the public domain.  ---Danny Sleator Nov. 2017

public class Simplex {
    double[][] A;

    int[] basic;    // size m.  indices of basic vars
    int[] nonbasic; // size n.  indices of non-basic vars

    int m, n;
    double[] soln;
    double z; // return value of the objective function.
    int lp_type; // for return.  1 if feasible, 0 if not feasible, -1 if unbounded

    final double INF = 1e100;
    final double EPS = 1e-9;
    final int FEASIBLE = 1;
    final int INFEASIBLE = 0;
    final int UNBOUNDED = -1;
  /*
    input:
      m = #constraints, n =#variables
      max c dot x s.t. a x <= b
      where a = mxn, b = m vector, c = n vector
    output:
      Infeasible, or Unbounded, or a pair Feasible (z,soln) where z is
      the maximum objective function value, and soln is an n-vector of
      variable values.
    caveats:
      Cycling is possible.  Nothing is done to mitigate loss of
      precision when the number of iterations is large.
  */
    Simplex(int m0, int n0, double[][] A0, double[] B, double[] C) {
	n = n0;
	m = m0;
	A = new double[m+1][n+1];
	basic = new int[m];
	nonbasic = new int[n];
	for (int j=0; j<m; j++) basic[j] = n+j;
	for (int i=0; i<n; i++) nonbasic[i] = i;

	for (int i=0; i<m; i++) {
	    A[i][n] = B[i];
	    for (int j=0; j<n; j++) A[i][j] = A0[i][j];
	}

	for (int j=0; j<n; j++) A[m][j] = C[j];

	if(!Feasible()) {
	    lp_type = INFEASIBLE;
	    return;
	}
	
	while(true) {
	    int r=0, c=0;
	    double p = 0.0;
	    for (int i=0; i<n; i++) {if (A[m][i] > p) p = A[m][c=i];}
	    if(p < EPS) {
		soln = new double[n];
		for(int j=0; j<n; j++) if (nonbasic[j] < n) soln[nonbasic[j]] = 0;
		for(int i=0; i<m; i++) if (basic[i] < n) soln[basic[i]] = A[i][n];
		z = -A[m][n];
		lp_type = FEASIBLE;
		break;
	    }
	    p = INF;
	    for (int i=0; i<m; i++) {
		if (A[i][c] > EPS) {
		    double v = A[i][n] / A[i][c];
		    if (v < p) {p = v; r = i;}
		}
	    }
	    if (p == INF) {
		lp_type = UNBOUNDED;
		break;
	    }
	    Pivot(r,c);
	}
    }

    private void Pivot(int r, int c) {
	int t = basic[r];
	basic[r] = nonbasic[c];
	nonbasic[c] = t;
	
	A[r][c] = 1 / A[r][c];
	for(int j=0; j<=n; j++) {
	    if (j!=c) A[r][j] *= A[r][c];
	}
	for(int i=0; i<=m; i++) {
	    if (i != r) {
		for (int j=0; j<=n; j++) {if (j!=c) A[i][j] -= A[i][c] * A[r][j];}
		A[i][c] = -A[i][c] * A[r][c];
	    }
	}
    }

    private boolean Feasible() {
	int r=0, c=0;
	while(true) {
	    double p = INF;
	    for(int i=0; i<m; i++) if(A[i][n] < p) p = A[r=i][n];
	    if(p > -EPS) return true;
	    p = 0.0;
	    for(int i=0; i<n; i++) if(A[r][i] < p) p = A[r][c=i];
	    if(p > -EPS) return false;
	    p = A[r][n] / A[r][c];
	    for(int i=r+1; i<m; i++) {
		if(A[i][c] > EPS) {
		    double v = A[i][n] / A[i][c];
		    if(v < p) {p = v; r = i;}
		}
	    }
	    Pivot(r,c);
	}
    }
    
    public static void main(String[] args) {

	double[][] A = new double[][] {{1.0, 3.0, 1.0},
				       {-1.0, 0.0, 3.0},
				       {2.0, -1.0, 2.0},
				       {2.0, 3.0, -1.0}};
	
	double[] B = new double[] {3.0, 2.0, 4.0, 2.0};

	double[] C = new double[] {5.0, 5.0, 3.0};

	int m = 4;
	int n = 3;
	
	Simplex lp = new Simplex(m, n, A, B, C);

	if (lp.lp_type == lp.UNBOUNDED) {
	    System.out.format("unbounded\n");
	} else if (lp.lp_type == lp.INFEASIBLE) {
	    System.out.format("infeasible\n");
	} else if (lp.lp_type == lp.FEASIBLE) {
	    System.out.format("The optimum is %f\n", lp.z);
	    for (int i = 0; i<n; i++) {
		System.out.format("x%d = %f\n", i, lp.soln[i]);
	    }
	} else {
	    System.out.format("Should not have happened");
	}
    }
}
