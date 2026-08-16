#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>
#include <stddef.h>

#define DO_NOT_OPTIMIZE(var) __asm__ volatile("" : : "g"(var) : "memory")
#define COMPILER_BARRIER() __asm__ volatile("" : : : "memory")

typedef struct {
    double mean_ns;
    double min_ns;
    double max_ns;
    double stddev_ns;
    double p50_ns;
    double p99_ns;
    size_t iterations;
} BenchStats;

typedef void (*BenchFunc)(void *ctx);

BenchStats run_benchmark(BenchFunc func, void *ctx, size_t warmup, size_t runs);
void print_stats(const char *name, BenchStats stats);

#endif