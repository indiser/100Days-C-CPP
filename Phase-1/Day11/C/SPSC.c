// Single Producer Single Consumer Queue Design
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<stdatomic.h>

#define CAPACITY 1024

typedef struct
{
    uint64_t data[CAPACITY];
    _Atomic size_t head, tail;
} LockFreeQueue;

void initialize(LockFreeQueue *q)
{
    atomic_store_explicit(&q->head, 0, memory_order_relaxed);
    atomic_store_explicit(&q->tail, 0, memory_order_relaxed);
}

bool queue_push(LockFreeQueue *q, uint64_t val)
{
    size_t tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    size_t head, next_tail;

    do {
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        next_tail = (tail + 1) % CAPACITY;

        if (next_tail == head) return false; // Full
    } while (!atomic_compare_exchange_weak_explicit(
        &q->tail, &tail, next_tail,
        memory_order_release, memory_order_relaxed));

    q->data[tail] = val;
    return true;
}

bool queue_pop(LockFreeQueue *q, uint64_t *val)
{
    size_t head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t tail, next_head;

    do {
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        if (head == tail) return false; // Empty

        next_head = (head + 1) % CAPACITY;
    } while (!atomic_compare_exchange_weak_explicit(
        &q->head, &head, next_head,
        memory_order_release, memory_order_relaxed));

    *val = q->data[head];
    return true;
}

int main(void)
{
    LockFreeQueue q;
    initialize(&q);

    uint64_t val = 1234;

    if(queue_push(&q, val))
    {
        printf("Pushed %lu\n", val);
    }
    else
    {
        printf("Push failed of %lu\n", val);
    }

    uint64_t val_pop = 0;
    if(queue_pop(&q, &val_pop))
    {
        printf("Popped %lu\n", val_pop);
    }
    else
    {
        printf("Pop failed of %lu\n", val_pop);
    }
    return 0;
}