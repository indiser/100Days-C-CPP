// matmul.c — big matrix multiply, three variants for perf comparison
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 1024
#define BLOCK 64

static double *A, *B, *C;

static void alloc_all(void) {
    A = malloc(sizeof(double) * N * N);
    B = malloc(sizeof(double) * N * N);
    C = malloc(sizeof(double) * N * N);
    if (!A || !B || !C) { fprintf(stderr, "alloc fail\n"); exit(1); }
}

static void fill(double *m) {
    for (int i = 0; i < N * N; i++) m[i] = (double)(i % 7) + 1.0;
}

static void zero(double *m) {
    for (int i = 0; i < N * N; i++) m[i] = 0.0;
}

// naive i,j,k order — bad access pattern on B
static void mul_naive(void) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++)
                sum += A[i*N + k] * B[k*N + j];
            C[i*N + j] = sum;
        }
}

// reordered i,k,j — sequential access on B, big free win
static void mul_reordered(void) {
    zero(C);
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++) {
            double a = A[i*N + k];
            for (int j = 0; j < N; j++)
                C[i*N + j] += a * B[k*N + j];
        }
}

// tiled/blocked — working set fits in cache per block
static void mul_tiled(void) {
    zero(C);
    for (int ii = 0; ii < N; ii += BLOCK)
        for (int kk = 0; kk < N; kk += BLOCK)
            for (int jj = 0; jj < N; jj += BLOCK)
                for (int i = ii; i < ii + BLOCK; i++)
                    for (int k = kk; k < kk + BLOCK; k++) {
                        double a = A[i*N + k];
                        for (int j = jj; j < jj + BLOCK; j++)
                            C[i*N + j] += a * B[k*N + j];
                    }
}

static double checksum(void) {
    double s = 0.0;
    for (int i = 0; i < N * N; i++) s += C[i];
    return s;
}

int main(int argc, char **argv) {
    // usage: ./matmul.exe [naive|reordered|tiled]
    const char *mode = argc > 1 ? argv[1] : "naive";

    alloc_all();
    fill(A);
    fill(B);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    if (strcmp(mode, "naive") == 0) mul_naive();
    else if (strcmp(mode, "reordered") == 0) mul_reordered();
    else if (strcmp(mode, "tiled") == 0) mul_tiled();
    else { fprintf(stderr, "unknown mode: %s\n", mode); return 1; }

    clock_gettime(CLOCK_MONOTONIC, &t1);

    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    fprintf(stderr, "mode=%s time=%.4fs checksum=%.1f\n", mode, secs, checksum());

    free(A); free(B); free(C);
    return 0;
}