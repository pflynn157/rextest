#include "rex_kmp.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <arm_sve.h> 
//#include <math.h>
#define N_RUNS 20
#define N 10240
// read timer in second

double read_timer()
{
  struct timeb tm;
  ftime(&tm);
  return ((double )tm . time) + ((double )tm . millitm) / 1000.0;
}
//Create a matrix and a vector and fill with random numbers

void init(float *matrix,float *vector)
{
  for (int i = 0; i < 10240; i++) {
    for (int j = 0; j < 10240; j++) {
      matrix[i * 10240 + j] = ((float )(rand())) / ((float )(2147483647 / 10.0));
    }
    vector[i] = ((float )(rand())) / ((float )(2147483647 / 10.0));
  }
}

void matvec_simd(float *matrix,float *vector,float *dest)
{
  int j;
  for (int i = 0; i < 10240; i++) {
    svfloat32_t __part0 = svdup_f32(0.00000L);
    float tmp = 0;
    svbool_t __pg0 = svwhilelt_b32((unsigned long )0,(unsigned long )((10239 < (_lt_var_j + 2 - 1))?10239 : (_lt_var_j + 2 - 1)));
    for (j = _lt_var_j; j <= (((10239 < (_lt_var_j + 2 - 1))?10239 : (_lt_var_j + 2 - 1))); j += 1 * svcntw()) {
      svfloat32_t __vec1 = svld1(__pg0,&matrix[i * 10240 + j]);
      svfloat32_t __vec2 = svld1(__pg0,&vector[j]);
      svfloat32_t __vec3 = svmul_f32_m(__pg0,__vec2,__vec1);
      svfloat32_t __vec4 = svadd_f32_m(__pg0,__vec3,__part0);
      __part0 = (__vec4);
      __pg0 = svwhilelt_b32((unsigned long )j,(unsigned long )(((10239 < (_lt_var_j + 2 - 1))?10239 : (_lt_var_j + 2 - 1))));
    }
    __pg0 = svptrue_b32();
    tmp += svaddv(__pg0,__part0);
    dest[i] = tmp;
  }
}
// Debug functions

void matvec_serial(float *matrix,float *vector,float *dest)
{
  for (int i = 0; i < 10240; i++) {
    float tmp = 0;
    for (int j = 0; j < 10240; j++) {
      tmp += matrix[i * 10240 + j] * vector[j];
    }
    dest[i] = tmp;
  }
}

float check(float *A,float *B)
{
  float difference = 0;
  for (int i = 0; i < 10240; i++) {
    difference += (fabsf((A[i] - B[i])));
  }
  return difference;
}

int main(int argc,char **argv)
{
  int status = 0;
//Set everything up
  float *dest_vector = (malloc(sizeof(float *) * 10240));
  float *serial_vector = (malloc(sizeof(float *) * 10240));
  float *matrix = (malloc(sizeof(float *) * 10240 * 10240));
  float *vector = (malloc(sizeof(float ) * 10240));
  srand((time(((void *)0))));
  init(matrix,vector);
//warming up
  matvec_simd(matrix,vector,dest_vector);
  double t = 0;
  double start = read_timer();
  for (int i = 0; i < 20; i++) {
    fprintf(stderr,"%d ",i);
    matvec_simd(matrix,vector,dest_vector);
    fprintf(stderr,"(%f,%f,%f)",dest_vector[0],dest_vector[10240 - 10],dest_vector[10240 / 10]);
  }
  fprintf(stderr,"\n");
  t += read_timer() - start;
  double t_serial = 0;
  double start_serial = read_timer();
  for (int i = 0; i < 20; i++) 
    matvec_serial(matrix,vector,serial_vector);
  t_serial += read_timer() - start_serial;
  double gflops = 2.0 * 10240 * 10240 * 20 / (1.0e9 * t);
  double gflops_serial = 2.0 * 10240 * 10240 * 20 / (1.0e9 * t_serial);
/*printf("==================================================================\n");
    printf("Performance:\t\t\tRuntime (s)\t GFLOPS\n");
    printf("------------------------------------------------------------------\n");
    printf("Matrix-vector (SIMD):\t\t%4f\t%4f\n", t/N_RUNS, gflops);
    printf("Matrix-vector (Serial):\t\t%4f\t%4f\n", t_serial/N_RUNS, gflops_serial);
    printf("Correctness check: %f\n", check(dest_vector,serial_vector));*/
  printf("%4f",t / 20);
  free(dest_vector);
  free(serial_vector);
  free(matrix);
  free(vector);
  return 0;
}
