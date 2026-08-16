#define _POSIX_C_SOURCE 199309L
#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

static int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

static double get_ns_diff(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1e9 + (double)(end.tv_nsec - start.tv_nsec);
}

BenchStats run_benchmark(BenchFunc func, void *ctx, size_t warmup, size_t runs) {
    if (runs == 0) {
        fprintf(stderr, "Runs must be > 0\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < warmup; i++) {
        func(ctx);
        COMPILER_BARRIER();
    }

    double *samples = malloc(runs * sizeof(double));
    if (!samples) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    double total_ns = 0.0;
    struct timespec t1, t2;

    for (size_t i = 0; i < runs; i++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
        func(ctx);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
        
        double diff = get_ns_diff(t1, t2);
        samples[i] = diff;
        total_ns += diff;
        COMPILER_BARRIER();
    }

    qsort(samples, runs, sizeof(double), compare_doubles);

    double mean = total_ns / (double)runs;
    double variance_sum = 0.0;
    for (size_t i = 0; i < runs; i++) {
        double diff = samples[i] - mean;
        variance_sum += diff * diff;
    }

    size_t p50_idx = runs / 2;
    size_t p99_idx = (size_t)(runs * 0.99);
    if (p99_idx >= runs) p99_idx = runs - 1;

    BenchStats stats = {
        .mean_ns = mean,
        .min_ns = samples[0],
        .max_ns = samples[runs - 1],
        .stddev_ns = sqrt(variance_sum / (double)runs),
        .p50_ns = samples[p50_idx],
        .p99_ns = samples[p99_idx],
        .iterations = runs
    };

    free(samples);
    return stats;
}

void print_stats(const char *name, BenchStats stats) {
    printf("--- Benchmark: %s ---\n", name);
    printf("Iterations : %zu\n", stats.iterations);
    printf("Mean       : %.2f ns\n", stats.mean_ns);
    printf("Min        : %.2f ns\n", stats.min_ns);
    printf("Max        : %.2f ns\n", stats.max_ns);
    printf("StdDev     : %.2f ns\n", stats.stddev_ns);
    printf("P50 (Med)  : %.2f ns\n", stats.p50_ns);
    printf("P99        : %.2f ns\n\n", stats.p99_ns);
}