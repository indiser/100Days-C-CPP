#include "benchmark.hpp"
#include <array>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <functional>

// Day 24 Open-Addressing Hash Map
constexpr std::size_t MAP_SIZE = 1024;

struct HashNode {
    std::uint64_t key;
    std::uint64_t val;
    bool occupied;
};

struct HashMap {
    std::array<HashNode, MAP_SIZE> nodes{};
    
    static std::uint64_t hash_fn(std::uint64_t x) {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    void insert(std::uint64_t k, std::uint64_t v) {
        std::size_t idx = hash_fn(k) % MAP_SIZE;
        while (nodes[idx].occupied) idx = (idx + 1) % MAP_SIZE;
        nodes[idx] = HashNode{k, v, true};
    }

    std::uint64_t find(std::uint64_t k) const {
        std::size_t idx = hash_fn(k) % MAP_SIZE;
        while (nodes[idx].occupied) {
            if (nodes[idx].key == k) return nodes[idx].val;
            idx = (idx + 1) % MAP_SIZE;
        }
        return 0;
    }
};

int main() {
    // Target 1: Cheap LCG Op captured by reference in closure
    std::uint64_t lcg_state = 42;
    auto lcg_lambda = [&lcg_state]() {
        lcg_state = (lcg_state * 6364136223846793005ULL) + 1442695040888963407ULL;
        DO_NOT_OPTIMIZE(lcg_state);
    };

    auto s1 = run_benchmark(lcg_lambda, 10000, 1000000);
    print_stats("Cheap Op (LCG - Raw Template)", s1);

    // Target 2: Medium Op (malloc/free round-trip)
    auto malloc_free_lambda = []() {
        void* ptr = std::malloc(256);
        DO_NOT_OPTIMIZE(ptr);
        std::free(ptr);
    };

    auto s2 = run_benchmark(malloc_free_lambda, 1000, 100000);
    print_stats("Medium Op (malloc/free 256B - Raw Template)", s2);

    // Target 3: Day 24 Hash Map Lookup (Rotates keys in captured map)
    HashMap map;
    std::array<std::uint64_t, 500> keys_to_find{};
    for (std::uint64_t i = 1; i <= 500; ++i) {
        map.insert(i, i * 100);
        keys_to_find[i - 1] = i;
    }

    std::size_t key_idx = 0;
    auto hashmap_lambda = [&map, &keys_to_find, &key_idx]() {
        std::uint64_t k = keys_to_find[key_idx];
        key_idx = (key_idx + 1) % 500;
        std::uint64_t val = map.find(k);
        DO_NOT_OPTIMIZE(val);
    };

    auto s3 = run_benchmark(hashmap_lambda, 10000, 1000000);
    print_stats("Day 24 Hash Map Lookup (Raw Template)", s3);

    // Target 4: std::function Type Erasure Overhead Comparison
    std::function<void()> erased_lcg = lcg_lambda;
    auto s4 = run_benchmark(erased_lcg, 10000, 1000000);
    print_stats("Cheap Op (LCG - std::function Overhead)", s4);

    // Target 5: Small Run Verification
    auto s5 = run_benchmark(lcg_lambda, 10, 100);
    assert(s5.p99_ns <= s5.max_ns);
    assert(s5.p50_ns <= s5.p99_ns);
    std::cout << "Verification passed: p50 (" << s5.p50_ns << "ns) <= p99 ("
              << s5.p99_ns << "ns) <= max (" << s5.max_ns << "ns)\n\n";

    return 0;
}