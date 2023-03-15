#include "rex_kmp.h" 
//axpy.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <immintrin.h> 
#define N_RUNS 20
#define N 102400000
// read timer in second

double read_timer()
{
  struct timeb tm;
  ftime(&tm);
  return ((double )tm . time) + ((double )tm . millitm) / 1000.0;
}
//Create a matrix and a vector and fill with random numbers

void init(float *X,float *Y)
{
  for (int i = 0; i < 102400000; i++) {
    X[i] = ((float )(rand())) / ((float )(2147483647 / 10.0));
    Y[i] = ((float )(rand())) / ((float )(2147483647 / 10.0));
  }
}
//Our sum function- what it does is pretty straight-forward.

void axpy(float *X,float *Y,float a)
{
  __m512 __vec16 = _mm512_set1_ps(a);
  __m512 __vec11 = _mm512_set1_ps(a);
  __m512 __vec6 = _mm512_set1_ps(a);
  __m512 __vec1 = _mm512_set1_ps(a);
  int i;
  for (i = _lt_var_i; i <= (((102399999 < (_lt_var_i + 2 - 1))?102399999 : (_lt_var_i + 2 - 1))); i += 4 * 16) {
    __m512 __vec0 = _mm512_loadu_ps(&Y[i]);
    __m512 __vec2 = _mm512_loadu_ps(&X[i]);
    __m512 __vec3 = _mm512_mul_ps(__vec2,__vec1);
    __m512 __vec4 = _mm512_add_ps(__vec3,__vec0);
    _mm512_storeu_ps(&Y[i],__vec4);
    __m512 __vec5 = _mm512_loadu_ps(&Y[i + 1]);
    __m512 __vec7 = _mm512_loadu_ps(&X[i + 1]);
    __m512 __vec8 = _mm512_mul_ps(__vec7,__vec6);
    __m512 __vec9 = _mm512_add_ps(__vec8,__vec5);
    _mm512_storeu_ps(&Y[i + 1],__vec9);
    __m512 __vec10 = _mm512_loadu_ps(&Y[i + 2]);
    __m512 __vec12 = _mm512_loadu_ps(&X[i + 2]);
    __m512 __vec13 = _mm512_mul_ps(__vec12,__vec11);
    __m512 __vec14 = _mm512_add_ps(__vec13,__vec10);
    _mm512_storeu_ps(&Y[i + 2],__vec14);
    __m512 __vec15 = _mm512_loadu_ps(&Y[i + 3]);
    __m512 __vec17 = _mm512_loadu_ps(&X[i + 3]);
    __m512 __vec18 = _mm512_mul_ps(__vec17,__vec16);
    __m512 __vec19 = _mm512_add_ps(__vec18,__vec15);
    _mm512_storeu_ps(&Y[i + 3],__vec19);
  }
}
// Debug functions

void axpy_serial(float *X,float *Y,float a)
{
  for (int i = 0; i < 102400000; i++) {
    Y[i] += a * X[i];
  }
}

float check(float *A,float *B)
{
  float difference = 0;
  for (int i = 0; i < 102400000; i++) {
    difference += A[i] - B[i];
  }
  return difference;
}

int main(int argc,char **argv)
{
  int status = 0;
//Set everything up
  float *X = (malloc(sizeof(float ) * 102400000));
  float *Y = (malloc(sizeof(float ) * 102400000));
  float *Y_serial = (malloc(sizeof(float ) * 102400000));
  float a = 3.14;
  srand((time(((void *)0))));
  init(X,Y);
  for (int i = 0; i < 102400000; i++) 
    Y_serial[i] = Y[i];
//warming up
  axpy(X,Y,a);
  axpy_serial(X,Y_serial,a);
  init(X,Y);
  for (int i = 0; i < 102400000; i++) 
    Y_serial[i] = Y[i];
  double t = 0;
  double start = read_timer();
  for (int i = 0; i < 20; i++) {
    fprintf(stderr,"%d ",i);
    axpy(X,Y,a);
    fprintf(stderr,"(%f,%f,%f)",Y[0],Y[102400000 - 10],Y[102400000 / 10]);
  }
  fprintf(stderr,"\n");
  t += read_timer() - start;
  double t_serial = 0;
  double start_serial = read_timer();
  for (int i = 0; i < 20; i++) 
    axpy_serial(X,Y_serial,a);
  t_serial += read_timer() - start_serial;
  double gflops = 2.0 * 102400000 * 102400000 * 20 / (1.0e9 * t);
  double gflops_serial = 2.0 * 102400000 * 102400000 * 20 / (1.0e9 * t_serial);
/*printf("==================================================================\n");
    printf("Performance:\t\t\tRuntime (s)\t GFLOPS\n");
    printf("------------------------------------------------------------------\n");
    printf("AXPY (SIMD):\t\t%4f\t%4f\n", t/N_RUNS, gflops);
    printf("AXPY (Serial):\t\t%4f\t%4f\n", t_serial/N_RUNS, gflops_serial);
    printf("Correctness check: %f\n", check(Y,Y_serial));*/
  printf("%4f",t / 20);
  free(X);
  free(Y);
  free(Y_serial);
  return 0;
}
