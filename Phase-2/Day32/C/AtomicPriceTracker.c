#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NUM_UPDATES 1000000
#define CACHE_LINE 64

typedef struct {
    _Atomic long price;
    char pad1[CACHE_LINE - sizeof(_Atomic long)];
    _Atomic long volume;
    char pad2[CACHE_LINE - sizeof(_Atomic long)];
    _Atomic long seq;
    char pad3[CACHE_LINE - sizeof(_Atomic long)];
} Ticker;

Ticker ticker;
_Atomic int stop_flag = 0;

void *writer_thread(void *arg) {
    (void)arg;
    long base_price = 10000; // fixed point, 2 decimals
    for (long i = 0; i < NUM_UPDATES; i++) {
        long new_price = base_price + (i % 500);
        long new_volume = (i * 7) % 1000;

        // publish payload first (relaxed ok, protected by release below)
        atomic_store_explicit(&ticker.price, new_price, memory_order_relaxed);
        atomic_store_explicit(&ticker.volume, new_volume, memory_order_relaxed);

        // release fence: everything above must be visible before this
        atomic_store_explicit(&ticker.seq, i + 1, memory_order_release);
    }
    atomic_store_explicit(&stop_flag, 1, memory_order_release);
    return NULL;
}

void *reader_thread(void *arg) {
    (void)arg;
    long last_seq = 0;
    long reads = 0;
    long torn_detected = 0;

    while (!atomic_load_explicit(&stop_flag, memory_order_acquire)) {
        long seq = atomic_load_explicit(&ticker.seq, memory_order_acquire);
        if (seq != last_seq) {
            long price = atomic_load_explicit(&ticker.price, memory_order_relaxed);
            long volume = atomic_load_explicit(&ticker.volume, memory_order_relaxed);
            (void)price;
            (void)volume;
            last_seq = seq;
            reads++;
        }
    }

    printf("reader: saw %ld distinct updates, torn=%ld\n", reads, torn_detected);
    return NULL;
}

// CAS retry loop: update price only if higher than current
int update_if_higher(_Atomic long *price, long candidate) {
    long current = atomic_load_explicit(price, memory_order_relaxed);
    while (candidate > current) {
        if (atomic_compare_exchange_weak_explicit(
                price, &current, candidate,
                memory_order_release, memory_order_relaxed)) {
            return 1; // updated
        }
        // current auto-refreshed by CAS on failure, loop retries
    }
    return 0; // not updated
}

double bench_ordering(memory_order store_order, memory_order load_order, long iters) {
    _Atomic long counter = 0;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (long i = 0; i < iters; i++) {
        atomic_store_explicit(&counter, i, store_order);
        atomic_load_explicit(&counter, load_order);
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    return iters / secs; // ops/sec
}

int main(void) {
    atomic_init(&ticker.price, 0);
    atomic_init(&ticker.volume, 0);
    atomic_init(&ticker.seq, 0);

    pthread_t w, r;
    pthread_create(&w, NULL, writer_thread, NULL);
    pthread_create(&r, NULL, reader_thread, NULL);

    pthread_join(w, NULL);
    pthread_join(r, NULL);

    // CAS demo
    _Atomic long best_price = 0;
    long candidates[] = {100, 250, 90, 300, 300, 500};
    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); i++) {
        int updated = update_if_higher(&best_price, candidates[i]);
        printf("candidate=%ld updated=%d best=%ld\n",
               candidates[i], updated,
               atomic_load_explicit(&best_price, memory_order_relaxed));
    }

    // ordering cost benchmark
    long bench_iters = 20000000;
    double seq_cst = bench_ordering(memory_order_seq_cst, memory_order_seq_cst, bench_iters);
    double rel_acq = bench_ordering(memory_order_release, memory_order_acquire, bench_iters);
    double relaxed = bench_ordering(memory_order_relaxed, memory_order_relaxed, bench_iters);

    printf("\nordering benchmark (%ld iters):\n", bench_iters);
    printf("seq_cst:        %.2f ops/sec\n", seq_cst);
    printf("release/acquire: %.2f ops/sec\n", rel_acq);
    printf("relaxed:        %.2f ops/sec\n", relaxed);

    return 0;
}