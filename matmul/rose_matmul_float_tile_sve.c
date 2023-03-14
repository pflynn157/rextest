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
#include <arm_sve.h> 
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
      temp = 0;
{
        int _lt_var_inc = 1;
        int _lt_var_k;
        for (_lt_var_k = 0; _lt_var_k <= 1023; _lt_var_k += _lt_var_inc * 2) {
          svfloat32_t __part0 = svdup_f32(0.00000L);
          svbool_t __pg0 = svwhilelt_b32((unsigned long )0,(unsigned long )((1023 < (_lt_var_k + _lt_var_inc * 2 - 1))?1023 : (_lt_var_k + _lt_var_inc * 2 - 1)));
          for (k = _lt_var_k; k <= (((1023 < (_lt_var_k + _lt_var_inc * 2 - 1))?1023 : (_lt_var_k + _lt_var_inc * 2 - 1))); k += 1 * svcntw()) {
            float *__ptr1 = A[i];
            svfloat32_t __vec2 = svld1(__pg0,&__ptr1[k]);
            float *__ptr3 = B[j];
            svfloat32_t __vec4 = svld1(__pg0,&__ptr3[k]);
            svfloat32_t __vec5 = svmul_f32_m(__pg0,__vec4,__vec2);
            svfloat32_t __vec6 = svadd_f32_m(__pg0,__vec5,__part0);
            __part0 = (__vec6);
            __pg0 = svwhilelt_b32((unsigned long )k,(unsigned long )(((1023 < (_lt_var_k + _lt_var_inc * 2 - 1))?1023 : (_lt_var_k + _lt_var_inc * 2 - 1))));
          }
          __pg0 = svptrue_b32();
          temp += svaddv(__pg0,__part0);
        }
      }
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
