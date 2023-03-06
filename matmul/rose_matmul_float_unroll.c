#include "rex_kmp.h" 
/*
 * Square matrix multiplication
 * A[N][N] * B[N][N] = C[N][N]
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <immintrin.h> 
#define N 1024
//#define N 16
// read timer in second

double read_timer()
{
  struct timeb tm;
  ftime(&tm);
  return ((double )tm . time) + ((double )tm . millitm) / 1000.0;
}

void init(float **A)
{
  int i;
  int j;
  for (i = 0; i < 1024; i++) {
    for (j = 0; j < 1024; j++) {
      A[i][j] = ((float )(rand())) / ((float )(2147483647 / 10.0));
    }
  }
}

void matmul_simd(float **A,float **B,float **C)
{
  int i;
  int j;
  int k;
  float temp;
  for (i = 0; i < 1024; i++) {
    for (j = 0; j < 1024; j++) {
      __m512 __part0 = _mm512_setzero_ps();
      temp = 0;
      for (k = 0; k <= 1023; k += 4 * 16) {
        float *__ptr1 = A[i];
        __m512 __vec2 = _mm512_loadu_ps(&__ptr1[k]);
        float *__ptr3 = B[j];
        __m512 __vec4 = _mm512_loadu_ps(&__ptr3[k]);
        __m512 __vec5 = _mm512_mul_ps(__vec4,__vec2);
        __m512 __vec6 = _mm512_add_ps(__vec5,__part0);
        __part0 = (__vec6);
        float *__ptr7 = A[i];
        __m512 __vec8 = _mm512_loadu_ps(&__ptr7[k + 1]);
        float *__ptr9 = B[j];
        __m512 __vec10 = _mm512_loadu_ps(&__ptr9[k + 1]);
        __m512 __vec11 = _mm512_mul_ps(__vec10,__vec8);
        __m512 __vec12 = _mm512_add_ps(__vec11,__part0);
        __part0 = (__vec12);
        float *__ptr13 = A[i];
        __m512 __vec14 = _mm512_loadu_ps(&__ptr13[k + 2]);
        float *__ptr15 = B[j];
        __m512 __vec16 = _mm512_loadu_ps(&__ptr15[k + 2]);
        __m512 __vec17 = _mm512_mul_ps(__vec16,__vec14);
        __m512 __vec18 = _mm512_add_ps(__vec17,__part0);
        __part0 = (__vec18);
        float *__ptr19 = A[i];
        __m512 __vec20 = _mm512_loadu_ps(&__ptr19[k + 3]);
        float *__ptr21 = B[j];
        __m512 __vec22 = _mm512_loadu_ps(&__ptr21[k + 3]);
        __m512 __vec23 = _mm512_mul_ps(__vec22,__vec20);
        __m512 __vec24 = _mm512_add_ps(__vec23,__part0);
        __part0 = (__vec24);
      }
      __m256 __buf0 = _mm512_extractf32x8_ps(__part0,0);
      __m256 __buf1 = _mm512_extractf32x8_ps(__part0,1);
      __buf1 = _mm256_add_ps(__buf0,__buf1);
      __buf1 = _mm256_hadd_ps(__buf1,__buf1);
      __buf1 = _mm256_hadd_ps(__buf1,__buf1);
      float __buf2[8];
      _mm256_storeu_ps(&__buf2,__buf1);
      temp += __buf2[0] + __buf2[6];
      C[i][j] = temp;
    }
  }
}

void matmul_serial(float **A,float **B,float **C)
{
  int i;
  int j;
  int k;
  float temp;
  for (i = 0; i < 1024; i++) {
    for (j = 0; j < 1024; j++) {
      temp = 0;
      for (k = 0; k < 1024; k++) {
        temp += A[i][k] * B[j][k];
      }
      C[i][j] = temp;
    }
  }
}

float check(float **A,float **B)
{
  float difference = 0;
  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 1024; j++) {
      difference += A[i][j] - B[i][j];
    }
  }
  return difference;
}
// Main

int main(int argc,char *argv[])
{
  int status = 0;
//Set everything up
  float **A = (malloc(sizeof(float *) * 1024));
  float **B = (malloc(sizeof(float *) * 1024));
  float **C_simd = (malloc(sizeof(float *) * 1024));
  float **C_serial = (malloc(sizeof(float *) * 1024));
  float **BT = (malloc(sizeof(float *) * 1024));
  for (int i = 0; i < 1024; i++) {
    A[i] = (malloc(sizeof(float ) * 1024));
    B[i] = (malloc(sizeof(float ) * 1024));
    C_simd[i] = (malloc(sizeof(float ) * 1024));
    C_serial[i] = (malloc(sizeof(float ) * 1024));
    BT[i] = (malloc(sizeof(float ) * 1024));
  }
  srand((time(((void *)0))));
  init(A);
  init(B);
  for (int line = 0; line < 1024; line++) {
    for (int col = 0; col < 1024; col++) {
      BT[line][col] = B[col][line];
    }
  }
  int i;
  int num_runs = 20;
//Warming up
  matmul_simd(A,BT,C_simd);
  matmul_serial(A,BT,C_serial);
  double elapsed = 0;
  double elapsed1 = read_timer();
  for (i = 0; i < num_runs; i++) {
    fprintf(stderr,"%d ",i);
    matmul_simd(A,BT,C_simd);
    fprintf(stderr,"(%f,%f,%f)",C_simd[0],C_simd[1024 - 10],C_simd[1024 / 10]);
  }
  fprintf(stderr,"\n");
  elapsed += read_timer() - elapsed1;
  double elapsed_serial = 0;
  double elapsed_serial1 = read_timer();
  for (i = 0; i < num_runs; i++) 
    matmul_serial(A,BT,C_serial);
  elapsed_serial += read_timer() - elapsed_serial1;
  double gflops_omp = 2.0 * 1024 * 1024 * 1024 * num_runs / (1.0e9 * elapsed);
  double gflops_serial = 2.0 * 1024 * 1024 * 1024 * num_runs / (1.0e9 * elapsed_serial);
/*printf("======================================================================================================\n");
    printf("\tMatrix Multiplication: A[N][N] * B[N][N] = C[N][N], N=%d\n", N);
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("Performance:\t\tRuntime (s)\t GFLOPS\n");
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("matmul_omp:\t\t%4f\t%4f\n", elapsed/num_runs, gflops_omp);
    printf("matmul_serial:\t\t%4f\t%4f\n", elapsed_serial/num_runs, gflops_serial);
    printf("Correctness check: %f\n", check(C_simd,C_serial));*/
  printf("%4f",elapsed / num_runs,(check(C_simd,C_serial)));
  return 0;
}
