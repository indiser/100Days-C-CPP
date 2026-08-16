#include "benchmark.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* Note: Indirect call overhead (`func(ctx)`) adds ~2-3 ns baseline noise.
   Dominates cheap operations like LCG; negligible for heavy ops. */

/* --- Target 1: Cheap LCG Op --- */
typedef struct {
    uint64_t state;
} LcgContext;

void target_lcg(void *ctx) {
    LcgContext *w = (LcgContext *)ctx;
    w->state = (w->state * 6364136223846793005ULL) + 1442695040888963407ULL;
    DO_NOT_OPTIMIZE(w->state);
}

/* --- Target 2: Medium Op (malloc/free round-trip) --- */
void target_malloc_free(void *ctx) {
    (void)ctx;
    void *ptr = malloc(256);
    DO_NOT_OPTIMIZE(ptr);
    free(ptr);
}

/* --- Target 3: Day 24 Hash Map Lookup (Multi-Key Average) --- */
#define MAP_SIZE 1024

typedef struct {
    uint64_t key;
    uint64_t val;
    int occupied;
} HashNode;

typedef struct {
    HashNode nodes[MAP_SIZE];
    uint64_t keys_to_find[500];
    size_t key_idx;
} HashMapContext;

static uint64_t hash_fn(uint64_t x) {
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

void target_hashmap_lookup(void *ctx) {
    HashMapContext *map = (HashMapContext *)ctx;
    uint64_t k = map->keys_to_find[map->key_idx];
    map->key_idx = (map->key_idx + 1) % 500; // Rotate through 500 keys

    size_t idx = hash_fn(k) % MAP_SIZE;
    while (map->nodes[idx].occupied) {
        if (map->nodes[idx].key == k) {
            DO_NOT_OPTIMIZE(map->nodes[idx].val);
            return;
        }
        idx = (idx + 1) % MAP_SIZE;
    }
}

int main(void) {
    /* Test 1: Cheap */
    LcgContext lcg = { .state = 42 };
    BenchStats s1 = run_benchmark(target_lcg, &lcg, 10000, 1000000);
    print_stats("Cheap Op (LCG)", s1);

    /* Test 2: Medium (Round-trip cost) */
    BenchStats s2 = run_benchmark(target_malloc_free, NULL, 1000, 100000);
    print_stats("Medium Op (malloc/free 256B round-trip)", s2);

    /* Test 3: Day 24 Hash Map Lookup (Rotates 500 keys across probes) */
    HashMapContext map_ctx;
    memset(&map_ctx, 0, sizeof(map_ctx));
    for (uint64_t i = 1; i <= 500; i++) {
        size_t idx = hash_fn(i) % MAP_SIZE;
        while (map_ctx.nodes[idx].occupied) idx = (idx + 1) % MAP_SIZE;
        map_ctx.nodes[idx] = (HashNode){ .key = i, .val = i * 100, .occupied = 1 };
        map_ctx.keys_to_find[i - 1] = i;
    }
    map_ctx.key_idx = 0;
    
    BenchStats s3 = run_benchmark(target_hashmap_lookup, &map_ctx, 10000, 1000000);
    print_stats("Day 24 Hash Map Lookup (Averaged keys)", s3);

    /* Test 4: Small run count verification with explicit assertion */
    BenchStats s4 = run_benchmark(target_lcg, &lcg, 10, 100);
    print_stats("Small Run Verification (100 runs)", s4);
    
    // Validate p99 math on n=100 (index 99 must be <= max)
    assert(s4.p99_ns <= s4.max_ns);
    assert(s4.p50_ns <= s4.p99_ns);
    printf("Verification passed: p50 (%.2fns) <= p99 (%.2fns) <= max (%.2fns)\n\n", 
           s4.p50_ns, s4.p99_ns, s4.max_ns);

    return 0;
}