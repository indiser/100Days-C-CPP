#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdalign.h>
#include <pthread.h>
#include <assert.h>

#define CAPACITY 1024
#define MASK (CAPACITY - 1)
#define NUM_OPS 100000000ULL

// Power of two check at compile time
_Static_assert((CAPACITY & (CAPACITY - 1)) == 0, "CAPACITY must be power of 2");

typedef struct {
    uint64_t data[CAPACITY];
    alignas(64) _Atomic size_t head;
    alignas(64) _Atomic size_t tail;
} LockFreeQueue;

void initialize(LockFreeQueue *q)
{
    atomic_store_explicit(&q->head, 0, memory_order_relaxed);
    atomic_store_explicit(&q->tail, 0, memory_order_relaxed);
}

bool queue_push(LockFreeQueue *q, uint64_t val)
{
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    size_t current_head = atomic_load_explicit(&q->head, memory_order_acquire);

    if ((current_tail - current_head) >= CAPACITY) {
        return false;
    }

    q->data[current_tail & MASK] = val;
    atomic_store_explicit(&q->tail, current_tail + 1, memory_order_release);
    return true;
}

bool queue_pop(LockFreeQueue *q, uint64_t *val)
{
    size_t current_head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if (current_head == current_tail) {
        return false;
    }

    *val = q->data[current_head & MASK];
    atomic_store_explicit(&q->head, current_head + 1, memory_order_release);
    return true;
}


void* producer(void *arg)
{
    LockFreeQueue *q = (LockFreeQueue*)arg;
    for (uint64_t i = 0; i < NUM_OPS; ++i) {
        while (!queue_push(q, i)) {
        }
    }
    return NULL;
}

void* consumer(void *arg)
{
    LockFreeQueue *q = (LockFreeQueue*)arg;
    uint64_t val = 0;
    for (uint64_t i = 0; i < NUM_OPS; ++i) {
        while (!queue_pop(q, &val)) {
        }
        assert(val == i);
    }
    return NULL;
}

int main(void)
{
    LockFreeQueue *q = malloc(sizeof(LockFreeQueue));
    assert(q != NULL);
    initialize(q);

    // Simulated wraparound test: start head/tail near SIZE_MAX
    size_t near_max = SIZE_MAX - 1000;
    atomic_store_explicit(&q->head, near_max, memory_order_relaxed);
    atomic_store_explicit(&q->tail, near_max, memory_order_relaxed);

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, q);
    pthread_create(&cons, NULL, consumer, q);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("Passed %llu ops stress test + wraparound check\n", NUM_OPS);

    free(q);
    return 0;
}