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

#define N 1024
//#define N 16

// read timer in second
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

void init(float **A) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = (float)rand()/(float)(RAND_MAX/10.0);
        }
    }
}

void matmul_serial(float **A, float **B, float **C) {
    int i,j,k;
    float temp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            temp = 0;
            for (k = 0; k < N; k++) {
                temp += A[i][k] * B[j][k];
            }
            C[i][j] = temp;
        }
    }
}

// Main
int main(int argc, char *argv[]) {
    //Set everything up
    float **A = malloc(sizeof(float*)*N);
    float **B = malloc(sizeof(float*)*N);
    float **C_serial = malloc(sizeof(float*)*N);
    float **BT = malloc(sizeof(float*)*N);
    
    for (int i = 0; i<N; i++) {
        A[i] = malloc(sizeof(float)*N);
        B[i] = malloc(sizeof(float)*N);
        C_serial[i] = malloc(sizeof(float)*N);
        BT[i] = malloc(sizeof(float)*N);
    }
    

    srand(time(NULL));
    init(A);
    init(B);
    for(int line = 0; line<N; line++){
        for(int col = 0; col<N; col++){
            BT[line][col] = B[col][line];
        }
    }
    int i;
    int num_runs = 20;
    
    //Warming up
    matmul_serial(A, BT, C_serial);

    double elapsed = 0;
    double elapsed1 = read_timer();
    for (i=0; i<num_runs; i++) {
        fprintf(stderr, "%d ", i);
        matmul_serial(A, BT, C_serial);
        fprintf(stderr, "(%f,%f,%f)", C_serial[0], C_serial[N-10], C_serial[N/10]);
    }
    fprintf(stderr, "\n");
    elapsed += (read_timer() - elapsed1);
    
    double gflops_omp = ((((2.0 * N) * N) * N * num_runs) / (1.0e9 * elapsed));
    
    printf("%4f", elapsed/num_runs);
    
    return 0;
}

