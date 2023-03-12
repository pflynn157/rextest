#include "rex_kmp.h" 
////Example of sparse matrix-vector multiply, using CSR (compressed sparse row format).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Add timing support
#include <sys/timeb.h>
#include <arm_sve.h> 
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
    svfloat32_t __part0 = svdup_f32(0.00000L);
    float sum = 0.0;
    svbool_t __pg0 = svwhilelt_b32((unsigned long )0,(unsigned long )(ia[row + 1] - 1));
    for (idx = ia[row]; idx <= (ia[row + 1] - 1); idx += 4 * svcntw()) {
      svfloat32_t __vec1 = svld1(__pg0,&a[idx]);
      svint32_t __vindex0 = svld1(__pg0,&ja[idx]);
      svfloat32_t __vec2 = svld1_gather_index(__pg0,x,__vindex0);
      svfloat32_t __vec3 = svmul_f32_m(__pg0,__vec2,__vec1);
      svfloat32_t __vec4 = svadd_f32_m(__pg0,__vec3,__part0);
      __part0 = (__vec4);
      svfloat32_t __vec5 = svld1(__pg0,&a[idx + 1]);
      svint32_t __vindex1 = svld1(__pg0,&ja[idx + 1]);
      svfloat32_t __vec6 = svld1_gather_index(__pg0,x,__vindex1);
      svfloat32_t __vec7 = svmul_f32_m(__pg0,__vec6,__vec5);
      svfloat32_t __vec8 = svadd_f32_m(__pg0,__vec7,__part0);
      __part0 = (__vec8);
      svfloat32_t __vec9 = svld1(__pg0,&a[idx + 2]);
      svint32_t __vindex2 = svld1(__pg0,&ja[idx + 2]);
      svfloat32_t __vec10 = svld1_gather_index(__pg0,x,__vindex2);
      svfloat32_t __vec11 = svmul_f32_m(__pg0,__vec10,__vec9);
      svfloat32_t __vec12 = svadd_f32_m(__pg0,__vec11,__part0);
      __part0 = (__vec12);
      svfloat32_t __vec13 = svld1(__pg0,&a[idx + 3]);
      svint32_t __vindex3 = svld1(__pg0,&ja[idx + 3]);
      svfloat32_t __vec14 = svld1_gather_index(__pg0,x,__vindex3);
      svfloat32_t __vec15 = svmul_f32_m(__pg0,__vec14,__vec13);
      svfloat32_t __vec16 = svadd_f32_m(__pg0,__vec15,__part0);
      __part0 = (__vec16);
      __pg0 = svwhilelt_b32((unsigned long )idx,(unsigned long )(ia[row + 1] - 1));
    }
    __pg0 = svptrue_b32();
    sum += svaddv(__pg0,__part0);
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
