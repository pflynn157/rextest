//axpy.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>

#define N_RUNS 20
#define N 102400000

// read timer in second
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

//Create a matrix and a vector and fill with random numbers
void init(float *X, float *Y) {
    for (int i = 0; i<N; i++) {
        X[i] = (float)rand()/(float)(RAND_MAX/10.0);
        Y[i] = (float)rand()/(float)(RAND_MAX/10.0);
    }
}

//Our sum function- what it does is pretty straight-forward.
void axpy(float *X, float *Y, float a) {
    for (int i = 0; i<N; i++) {
        Y[i] += a * X[i];
    }
}

int main(int argc, char **argv) {
    //Set everything up
    float *X = malloc(sizeof(float)*N);
    float *Y = malloc(sizeof(float)*N);
    float a = 3.14;
    
    srand(time(NULL));
    init(X, Y);
    
    //warming up
    axpy(X, Y, a);
    init(X, Y);
    
    double t = 0;
    double start = read_timer();
    for (int i = 0; i<N_RUNS; i++) {
        fprintf(stderr, "%d ", i);
        axpy(X, Y, a);
        fprintf(stderr, "(%f,%f,%f)", Y[0], Y[N-10], Y[N/10]);
    }
    fprintf(stderr, "\n");
    t += (read_timer() - start);
    
    double gflops = ((2.0 * N) * N * N_RUNS) / (1.0e9 * t);
    
    printf("%4f,\n", t/N_RUNS);
    
    free(X);
    free(Y);
    
    return 0;    
}

