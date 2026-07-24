#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include <pthread.h>
#include <time.h>

#define ITERS 200000000UL

// BAD: no padding, adjacent ticks share cache line
typedef struct {
    uint64_t counter;
} TickBad;

// GOOD: forced to own cache line, no false sharing
typedef struct __attribute__((aligned(64))) {
    uint64_t counter;
} TickGood;

TickBad bad_arr[2];
TickGood good_arr[2];

void *bump_bad(void *arg) {
    int idx = *(int*)arg;
    for (uint64_t i = 0; i < ITERS; i++) {
        bad_arr[idx].counter++;
    }
    return NULL;
}

void *bump_good(void *arg) {
    int idx = *(int*)arg;
    for (uint64_t i = 0; i < ITERS; i++) {
        good_arr[idx].counter++;
    }
    return NULL;
}

double run(void *(*fn)(void*)) {
    pthread_t t0, t1;
    int a0 = 0, a1 = 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_create(&t0, NULL, fn, &a0);
    pthread_create(&t1, NULL, fn, &a1);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    printf("sizeof(TickBad)  = %zu, alignof = %zu\n", sizeof(TickBad), alignof(TickBad));
    printf("sizeof(TickGood) = %zu, alignof = %zu\n", sizeof(TickGood), alignof(TickGood));
    printf("bad_arr[0] addr: %p, bad_arr[1] addr: %p, diff: %ld\n",
           (void*)&bad_arr[0], (void*)&bad_arr[1],
           (long)((char*)&bad_arr[1] - (char*)&bad_arr[0]));
    printf("good_arr[0] addr: %p, good_arr[1] addr: %p, diff: %ld\n\n",
           (void*)&good_arr[0], (void*)&good_arr[1],
           (long)((char*)&good_arr[1] - (char*)&good_arr[0]));

    double t_bad = run(bump_bad);
    printf("False-sharing (unaligned) time: %.3f sec\n", t_bad);

    double t_good = run(bump_good);
    printf("Cache-aligned (no false-sharing) time: %.3f sec\n", t_good);

    printf("\nSlowdown factor from false sharing: %.2fx\n", t_bad / t_good);

    return 0;
}