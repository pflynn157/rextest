#include "rex_kmp.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <immintrin.h> 
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
    float tmp = 0;
{
      int _lt_var_j;
      for (_lt_var_j = 0; _lt_var_j <= 10239; _lt_var_j += 2) {
        __m512 __part0 = _mm512_setzero_ps();
        for (j = _lt_var_j; j <= (((10239 < (_lt_var_j + 2 - 1))?10239 : (_lt_var_j + 2 - 1))); j += 1 * 16) {
          __m512 __vec1 = _mm512_loadu_ps(&matrix[i * 10240 + j]);
          __m512 __vec2 = _mm512_loadu_ps(&vector[j]);
          __m512 __vec3 = _mm512_mul_ps(__vec2,__vec1);
          __m512 __vec4 = _mm512_add_ps(__vec3,__part0);
          __part0 = (__vec4);
        }
        __m256 __buf0 = _mm512_extractf32x8_ps(__part0,0);
        __m256 __buf1 = _mm512_extractf32x8_ps(__part0,1);
        __buf1 = _mm256_add_ps(__buf0,__buf1);
        __buf1 = _mm256_hadd_ps(__buf1,__buf1);
        __buf1 = _mm256_hadd_ps(__buf1,__buf1);
        float __buf2[8];
        _mm256_storeu_ps(&__buf2,__buf1);
        tmp += __buf2[0] + __buf2[6];
      }
    }
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
