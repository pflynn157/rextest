////Example of sparse matrix-vector multiply, using CSR (compressed sparse row format).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Add timing support
#include <sys/timeb.h>

#define REAL float

double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}


//#define DEFAULT_DIMSIZE 256

void print_array(char *title, char *name, REAL *A, int n, int m) {
    fprintf(stderr, "%s:\n", title);
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            fprintf(stderr, "%s[%d][%d]:%f  ", name, i, j, A[i * m + j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

/*  subroutine error_check (n,m,alpha,dx,dy,u,f)
 implicit none
 ************************************************************
 * Checks error between numerical and exact solution
 *
 ************************************************************/

int main(int argc, char *argv[]) {
    int    *ia, *ja;
    REAL *a, *x, *y;
    int    row, i, j, idx, n, nnzMax, nnz, nrows;
    REAL ts, t, rate;
    n = 10240;
    //n = 24;
    if (argc > 1) n = atoi(argv[1]);

    nrows  = n * n;
    nnzMax = nrows * 5;
    ia = (int*)malloc(nrows*sizeof(int));
    ja = (int*)malloc(nnzMax*sizeof(int));
    a  = (REAL*)malloc(nnzMax*sizeof(REAL));
    /* Allocate the source and result vectors */
    x = (REAL*)malloc(nrows*sizeof(REAL));
    y = (REAL*)malloc(nrows*sizeof(REAL));

    row = 0;
    nnz = 0;
    for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			ia[row] = nnz;
			if (i>0) { ja[nnz] = row - n; a[nnz] = -1.0; nnz++; }
			if (j>0) { ja[nnz] = row - 1; a[nnz] = -1.0; nnz++; }
			ja[nnz] = row; a[nnz] = 4.0; nnz++;
			if (j<n-1) { ja[nnz] = row + 1; a[nnz] = -1.0; nnz++; }
			if (i<n-1) { ja[nnz] = row + n; a[nnz] = -1.0; nnz++; }
			row++;
		}
    }
    ia[row] = nnz;

    /* Create the source (x) vector */
    for (i=0; i<nrows; i++) x[i] = 1.0;

    double elapsed = read_timer();
    
    for (row=0; row<nrows; row++) {
		REAL sum = 0.0;
		#pragma omp tile sizes(2,2)
		#pragma omp simd reduction(+:sum)
		for (idx=ia[row]; idx<ia[row+1]; idx++) {
			sum += a[idx] * x[ja[idx]];
		}
		y[row] = sum;
    }
    elapsed = read_timer() - elapsed;
        
    //printf("seq elasped time(s): %.4f\n", elapsed);
  
    int errors = 0;
    for (row=0; row<nrows; row++) {
		if (y[row] < 0) {
			//fprintf(stderr,"y[%d]=%f, fails consistency test\n", row, y[row]);
			++errors;
		}
    }
    fprintf(stderr, "Errors: %d\n", errors);
    
    printf("%.4f", elapsed);
    
    free(ia); free(ja); free(a); free(x); free(y);
    return 0;
}
