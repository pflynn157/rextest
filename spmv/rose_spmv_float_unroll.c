#include "rex_kmp.h" 
////Example of sparse matrix-vector multiply, using CSR (compressed sparse row format).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Add timing support
#include <sys/timeb.h>
#include <immintrin.h> 
#define REAL float

double read_timer()
{
  struct timeb tm;
  ftime(&tm);
  return ((double )tm . time) + ((double )tm . millitm) / 1000.0;
}
//#define DEFAULT_DIMSIZE 256

void print_array(char *title,char *name,float *A,int n,int m)
{
  fprintf(stderr,"%s:\n",title);
  int i;
  int j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      fprintf(stderr,"%s[%d][%d]:%f  ",name,i,j,A[i * m + j]);
    }
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"\n");
}
/*  subroutine error_check (n,m,alpha,dx,dy,u,f)
 implicit none
 ************************************************************
 * Checks error between numerical and exact solution
 *
 ************************************************************/

int main(int argc,char *argv[])
{
  int status = 0;
  int *ia;
  int *ja;
  float *a;
  float *x;
  float *y;
  int row;
  int i;
  int j;
  int idx;
  int n;
  int nnzMax;
  int nnz;
  int nrows;
  float ts;
  float t;
  float rate;
  n = 10240;
//n = 24;
  if (argc > 1) 
    n = atoi(argv[1]);
  nrows = n * n;
  nnzMax = nrows * 5;
  ia = ((int *)(malloc(nrows * sizeof(int ))));
  ja = ((int *)(malloc(nnzMax * sizeof(int ))));
  a = ((float *)(malloc(nnzMax * sizeof(float ))));
/* Allocate the source and result vectors */
  x = ((float *)(malloc(nrows * sizeof(float ))));
  y = ((float *)(malloc(nrows * sizeof(float ))));
  row = 0;
  nnz = 0;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      ia[row] = nnz;
      if (i > 0) {
        ja[nnz] = row - n;
        a[nnz] = (- 1.0);
        nnz++;
      }
      if (j > 0) {
        ja[nnz] = row - 1;
        a[nnz] = (- 1.0);
        nnz++;
      }
      ja[nnz] = row;
      a[nnz] = 4.0;
      nnz++;
      if (j < n - 1) {
        ja[nnz] = row + 1;
        a[nnz] = (- 1.0);
        nnz++;
      }
      if (i < n - 1) {
        ja[nnz] = row + n;
        a[nnz] = (- 1.0);
        nnz++;
      }
      row++;
    }
  }
  ia[row] = nnz;
/* Create the source (x) vector */
  for (i = 0; i < nrows; i++) 
    x[i] = 1.0;
  double elapsed = read_timer();
  for (row = 0; row < nrows; row++) {
    __m512 __part0 = _mm512_setzero_ps();
    float sum = 0.0;
    __mmask16 __mask0;
    __mmask16 __mask1;
    __mmask16 __mask2 = _kxnor_mask16(__mask0,__mask1);
    __m512 __buf0 = _mm512_setzero_ps();
    __mmask16 __mask3;
    __mmask16 __mask4;
    __mmask16 __mask5 = _kxnor_mask16(__mask3,__mask4);
    __m512 __buf1 = _mm512_setzero_ps();
    __mmask16 __mask6;
    __mmask16 __mask7;
    __mmask16 __mask8 = _kxnor_mask16(__mask6,__mask7);
    __m512 __buf2 = _mm512_setzero_ps();
    __mmask16 __mask9;
    __mmask16 __mask10;
    __mmask16 __mask11 = _kxnor_mask16(__mask9,__mask10);
    __m512 __buf3 = _mm512_setzero_ps();
    for (idx = ia[row]; idx <= ia[row + 1] - 1; idx += 4 * 16) {
      __m512 __vec1 = _mm512_loadu_ps(&a[idx]);
      __m512i __vindex0 = _mm512_loadu_si512((__m512i *)(&ja[idx]));
      __m512 __vec2 = _mm512_mask_i32gather_ps(__buf0,__mask2,__vindex0,x,4);
      __m512 __vec3 = _mm512_mul_ps(__vec2,__vec1);
      __m512 __vec4 = _mm512_add_ps(__vec3,__part0);
      __part0 = (__vec4);
      __m512 __vec5 = _mm512_loadu_ps(&a[idx + 1]);
      __m512i __vindex1 = _mm512_loadu_si512((__m512i *)(&ja[idx + 1]));
      __m512 __vec6 = _mm512_mask_i32gather_ps(__buf1,__mask5,__vindex1,x,4);
      __m512 __vec7 = _mm512_mul_ps(__vec6,__vec5);
      __m512 __vec8 = _mm512_add_ps(__vec7,__part0);
      __part0 = (__vec8);
      __m512 __vec9 = _mm512_loadu_ps(&a[idx + 2]);
      __m512i __vindex2 = _mm512_loadu_si512((__m512i *)(&ja[idx + 2]));
      __m512 __vec10 = _mm512_mask_i32gather_ps(__buf2,__mask8,__vindex2,x,4);
      __m512 __vec11 = _mm512_mul_ps(__vec10,__vec9);
      __m512 __vec12 = _mm512_add_ps(__vec11,__part0);
      __part0 = (__vec12);
      __m512 __vec13 = _mm512_loadu_ps(&a[idx + 3]);
      __m512i __vindex3 = _mm512_loadu_si512((__m512i *)(&ja[idx + 3]));
      __m512 __vec14 = _mm512_mask_i32gather_ps(__buf3,__mask11,__vindex3,x,4);
      __m512 __vec15 = _mm512_mul_ps(__vec14,__vec13);
      __m512 __vec16 = _mm512_add_ps(__vec15,__part0);
      __part0 = (__vec16);
    }
    __m256 __buf4 = _mm512_extractf32x8_ps(__part0,0);
    __m256 __buf5 = _mm512_extractf32x8_ps(__part0,1);
    __buf5 = _mm256_add_ps(__buf4,__buf5);
    __buf5 = _mm256_hadd_ps(__buf5,__buf5);
    __buf5 = _mm256_hadd_ps(__buf5,__buf5);
    float __buf6[8];
    _mm256_storeu_ps(&__buf6,__buf5);
    sum += __buf6[0] + __buf6[6];
    y[row] = sum;
  }
  elapsed = read_timer() - elapsed;
//printf("seq elasped time(s): %.4f\n", elapsed);
  int errors = 0;
  for (row = 0; row < nrows; row++) {
    if (y[row] < 0) {
//fprintf(stderr,"y[%d]=%f, fails consistency test\n", row, y[row]);
      ++errors;
    }
  }
//printf("Errors: %d\n", errors);
  printf("%.4f",elapsed);
  free(ia);
  free(ja);
  free(a);
  free(x);
  free(y);
  return 0;
}
