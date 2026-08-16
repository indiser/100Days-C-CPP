#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#define DO_NOT_OPTIMIZE(var) asm volatile("" : : "g"(var) : "memory")
#define COMPILER_BARRIER() asm volatile("" : : : "memory")

struct BenchStats {
    double mean_ns;
    double min_ns;
    double max_ns;
    double stddev_ns;
    double p50_ns;
    double p99_ns;
    std::size_t iterations;
};

// Raw template — zero virtual/indirect call overhead, direct lambda inlining
template <typename Callable>
BenchStats run_benchmark(Callable&& fn, std::size_t warmup, std::size_t runs) {
    if (runs == 0) {
        throw std::invalid_argument("Runs must be > 0");
    }

    // Warm-up phase
    for (std::size_t i = 0; i < warmup; ++i) {
        fn();
        COMPILER_BARRIER();
    }

    std::vector<double> samples;
    samples.reserve(runs);

    double total_ns = 0.0;

    for (std::size_t i = 0; i < runs; ++i) {
        auto t1 = std::chrono::steady_clock::now();
        fn();
        auto t2 = std::chrono::steady_clock::now();

        double diff = std::chrono::duration<double, std::nano>(t2 - t1).count();
        samples.push_back(diff);
        total_ns += diff;
        COMPILER_BARRIER();
    }

    std::sort(samples.begin(), samples.end());

    double mean = total_ns / static_cast<double>(runs);
    double variance_sum = 0.0;
    for (double s : samples) {
        double diff = s - mean;
        variance_sum += diff * diff;
    }

    std::size_t p50_idx = runs / 2;
    std::size_t p99_idx = static_cast<std::size_t>(runs * 0.99);
    if (p99_idx >= runs) p99_idx = runs - 1;

    return BenchStats{
        .mean_ns = mean,
        .min_ns = samples.front(),
        .max_ns = samples.back(),
        .stddev_ns = std::sqrt(variance_sum / static_cast<double>(runs)),
        .p50_ns = samples[p50_idx],
        .p99_ns = samples[p99_idx],
        .iterations = runs
    };
}

inline void print_stats(const char* name, const BenchStats& stats) {
    std::cout << "--- Benchmark: " << name << " ---\n"
              << "Iterations : " << stats.iterations << "\n"
              << "Mean       : " << std::fixed << std::setprecision(2) << stats.mean_ns << " ns\n"
              << "Min        : " << stats.min_ns << " ns\n"
              << "Max        : " << stats.max_ns << " ns\n"
              << "StdDev     : " << stats.stddev_ns << " ns\n"
              << "P50 (Med)  : " << stats.p50_ns << " ns\n"
              << "P99        : " << stats.p99_ns << " ns\n\n";
}

#endif