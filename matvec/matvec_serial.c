#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <math.h>

#define N_RUNS 20
#define N 10240

// read timer in second
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

//Create a matrix and a vector and fill with random numbers
void init(float *matrix, float *vector) {
    for (int i = 0; i<N; i++) {
        for (int j = 0; j<N; j++) {
            matrix[i*N+j] = (float)rand()/(float)(RAND_MAX/10.0);
        }
        
        vector[i] = (float)rand()/(float)(RAND_MAX/10.0);
    }
}

void matvec_serial(float *matrix, float *vector, float *dest) {
    for (int i = 0; i<N; i++) {
        float tmp = 0;
        for (int j = 0; j<N; j++) {
            tmp += matrix[i*N+j] * vector[j];
        }
        dest[i] = tmp;
    }
}

int main(int argc, char **argv) {
    //Set everything up
    float *dest_vector = malloc(sizeof(float*)*N);
    float *matrix = malloc(sizeof(float*)*N*N);
    float *vector = malloc(sizeof(float)*N);
    
    srand(time(NULL));
    init(matrix, vector);
    
    //warming up
    matvec_serial(matrix, vector, dest_vector);
    
    
    double t = 0;
    double start = read_timer();
    for (int i = 0; i<N_RUNS; i++) {
        fprintf(stderr, "%d ", i);
        matvec_serial(matrix, vector, dest_vector);
        fprintf(stderr, "(%f,%f,%f)", dest_vector[0], dest_vector[N-10], dest_vector[N/10]);
    }
    fprintf(stderr, "\n");
    t += (read_timer() - start);
    
    double gflops = ((2.0 * N) * N * N_RUNS) / (1.0e9 * t);
    
    printf("%4f", t/N_RUNS);
    
    free(dest_vector);
    free(matrix);
    free(vector);
    
    return 0;    
}

