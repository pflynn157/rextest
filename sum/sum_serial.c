//sum.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>

#define N_RUNS 20
#define N 10240000

// read timer in second
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

//Create a matrix and a vector and fill with random numbers
void init(float *X) {
    for (int i = 0; i<N; i++) {
        X[i] = (float)rand()/(float)(RAND_MAX/10.0);
    }
}

//Our sum function- what it does is pretty straight-forward.
float sum(float *X) {
    float result = 0;
    
    for (int i = 0; i<N; i++) {
        result += X[i];
    }
    
    return result;
}

int main(int argc, char **argv) {
    //Set everything up
    float *X = malloc(sizeof(float)*N);
    float result;
    
    srand(time(NULL));
    init(X);
    
    //warming up
    result = sum(X);
    
    double t = 0;
    double start = read_timer();
    for (int i = 0; i<N_RUNS; i++) {
        fprintf(stderr, "%d ", i);
        result = sum(X);
        fprintf(stderr, "(%f)", result);
    }
    fprintf(stderr, "\n");
    t += (read_timer() - start);
    
    double gflops = ((2.0 * N) * N * N_RUNS) / (1.0e9 * t);
    
    printf("%4f,%f\n", t/N_RUNS);
    
    free(X);
    
    return 0;    
}

