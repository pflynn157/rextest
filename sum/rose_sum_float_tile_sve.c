#include "rex_kmp.h" 
//sum.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <arm_sve.h> 
#define N_RUNS 20
#define N 10240000
// read timer in second

double read_timer()
{
  struct timeb tm;
  ftime(&tm);
  return ((double )tm . time) + ((double )tm . millitm) / 1000.0;
}
//Create a matrix and a vector and fill with random numbers

void init(float *X)
{
  for (int i = 0; i < 10240000; i++) {
    X[i] = ((float )(rand())) / ((float )(2147483647 / 10.0));
  }
}
//Our sum function- what it does is pretty straight-forward.

float sum(float *X)
{
  int i;
  float result = 0;
{
    int _lt_var_inc = 1;
    int _lt_var_i;
    for (_lt_var_i = 0; _lt_var_i <= 10239999; _lt_var_i += _lt_var_inc * 2) {
      svfloat32_t __part0 = svdup_f32(0.00000L);
      svbool_t __pg0 = svwhilelt_b32((unsigned long )0,(unsigned long )((10239999 < (_lt_var_i + _lt_var_inc * 2 - 1))?10239999 : (_lt_var_i + _lt_var_inc * 2 - 1)));
      for (i = _lt_var_i; i <= (((10239999 < (_lt_var_i + _lt_var_inc * 2 - 1))?10239999 : (_lt_var_i + _lt_var_inc * 2 - 1))); i += 1 * svcntw()) {
        svfloat32_t __vec1 = svld1(__pg0,&X[i]);
        svfloat32_t __vec2 = svadd_f32_m(__pg0,__vec1,__part0);
        __part0 = (__vec2);
        __pg0 = svwhilelt_b32((unsigned long )i,(unsigned long )(((10239999 < (_lt_var_i + _lt_var_inc * 2 - 1))?10239999 : (_lt_var_i + _lt_var_inc * 2 - 1))));
      }
      __pg0 = svptrue_b32();
      result += svaddv(__pg0,__part0);
    }
  }
  return result;
}
// Debug functions

float sum_serial(float *X)
{
  float result = 0;
  for (int i = 0; i < 10240000; i++) {
    result += X[i];
  }
  return result;
}

int main(int argc,char **argv)
{
  int status = 0;
//Set everything up
  float *X = (malloc(sizeof(float ) * 10240000));
  float result;
  float result_serial;
  srand((time(((void *)0))));
  init(X);
//warming up
  result = sum(X);
  result_serial = sum_serial(X);
  double t = 0;
  double start = read_timer();
  for (int i = 0; i < 20; i++) {
    fprintf(stderr,"%d ",i);
    result = sum(X);
    fprintf(stderr,"(%f)",result);
  }
  fprintf(stderr,"\n");
  t += read_timer() - start;
  double t_serial = 0;
  double start_serial = read_timer();
  for (int i = 0; i < 20; i++) 
    result_serial = sum_serial(X);
  t_serial += read_timer() - start_serial;
  double gflops = 2.0 * 10240000 * 10240000 * 20 / (1.0e9 * t);
  double gflops_serial = 2.0 * 10240000 * 10240000 * 20 / (1.0e9 * t_serial);
/*printf("==================================================================\n");
    printf("Performance:\t\t\tRuntime (s)\t GFLOPS\n");
    printf("------------------------------------------------------------------\n");
    printf("Sum (SIMD):\t\t%4f\t%4f\n", t/N_RUNS, gflops);
    printf("Sum (Serial):\t\t%4f\t%4f\n", t_serial/N_RUNS, gflops_serial);
    printf("Correctness check: %f\n", result_serial - result);*/
  printf("%4f",t / 20);
  free(X);
  return 0;
}
