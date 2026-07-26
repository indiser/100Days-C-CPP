// Multi Producer Multi Consumer using Dmitry Vyukov MPMC bounded Queue design with slots
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<stdatomic.h>
#include<pthread.h>
#include<assert.h>

#define CAPACITY 1024
#define NUM_THREADS 4
#define ITEMS_PER_THREAD 100000
#define TOTAL_ITEMS (NUM_THREADS * ITEMS_PER_THREAD)

typedef struct
{
    uint64_t data;
    _Atomic size_t sequence;
} Slot;

typedef struct
{
    Slot slots[CAPACITY];
    _Atomic size_t head, tail;
} LockFreeQueue;

void initialize(LockFreeQueue *q)
{
    for (size_t i = 0; i < CAPACITY; i++)
    {
        q->slots[i].data = 0;
        atomic_store_explicit(&q->slots[i].sequence, i, memory_order_relaxed);
    }
    atomic_store_explicit(&q->head, 0, memory_order_relaxed);
    atomic_store_explicit(&q->tail, 0, memory_order_relaxed);
}

bool queue_push(LockFreeQueue *q, uint64_t val)
{ 
    size_t pos = atomic_load_explicit(&q->tail, memory_order_relaxed);
    
    while(1)
    {
        Slot *slot = &q->slots[pos % CAPACITY];
        size_t seq = atomic_load_explicit(&slot->sequence, memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)pos;

        if(diff == 0) //Ready to write
        {
            if(atomic_compare_exchange_weak_explicit(&q->tail, &pos, pos + 1, memory_order_relaxed, memory_order_relaxed))
            {
                slot->data = val;
                atomic_store_explicit(&slot->sequence, pos + 1, memory_order_release);
                return true;
            }
        }
        else if(diff < 0) return false;
        else pos = atomic_load_explicit(&q->tail, memory_order_relaxed);
    }
}

bool queue_pop(LockFreeQueue *q, uint64_t *val)
{
    size_t pos= atomic_load_explicit(&q->head, memory_order_relaxed);

    while(1)
    {
        Slot *slot = &q->slots[pos % CAPACITY];
        size_t seq = atomic_load_explicit(&slot->sequence, memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

        if(diff == 0)
        {
            if(atomic_compare_exchange_weak_explicit(&q->head, &pos, pos + 1, memory_order_relaxed, memory_order_relaxed))
            {
                *val = slot->data;
                atomic_store_explicit(&slot->sequence, pos + CAPACITY, memory_order_release);
                return true;
            }
        }
        else if(diff < 0) return false;
        else pos = atomic_load_explicit(&q->head, memory_order_relaxed);
    }
}


// Test

_Atomic uint8_t received[TOTAL_ITEMS + 1];
_Atomic uint64_t total_consumed_sum = 0;


typedef struct {
    LockFreeQueue *q;
    size_t thread_id;
} ThreadArg;


void *producer(void *arg) {
    ThreadArg *t = (ThreadArg *)arg;
    for (size_t i = 0; i < ITEMS_PER_THREAD; i++) {
        uint64_t val = (t->thread_id * ITEMS_PER_THREAD) + i + 1;
        while (!queue_push(t->q, val));
    }
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    size_t popped_count = 0;
    uint64_t val;
    
    while (popped_count < ITEMS_PER_THREAD) {
        if (queue_pop(&((ThreadArg*)arg)->q[0], &val)) {
            popped_count++;
            
            // Check range
            assert(val >= 1 && val <= TOTAL_ITEMS);
            
            // Detect duplicates / corrupted values
            uint8_t prev = atomic_fetch_add_explicit(&received[val], 1, memory_order_relaxed);
            assert(prev == 0 && "DUPLICATE OR CORRUPTED VALUE RECEIVED!");
            
            atomic_fetch_add_explicit(&total_consumed_sum, val, memory_order_relaxed);
        }
    }
    return NULL;
}

int main()
{
    LockFreeQueue q;
    initialize(&q);

    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];
    ThreadArg args[NUM_THREADS];

    uint64_t expected_sum = 0;
    for (uint64_t i = 1; i <= TOTAL_ITEMS; i++) {
        expected_sum += i;
    }

    for (size_t i = 0; i < NUM_THREADS; i++) {
        args[i].q = &q;
        args[i].thread_id = i;
        pthread_create(&producers[i], NULL, producer, &args[i]);
        pthread_create(&consumers[i], NULL, consumer, &args[i]);
    }

    for (size_t i = 0; i < NUM_THREADS; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    // Verification
    for (size_t i = 1; i <= TOTAL_ITEMS; i++) {
        assert(received[i] == 1 && "MISSING ITEM DETECTED!");
    }
    assert(total_consumed_sum == expected_sum && "SUM MISMATCH DETECTED!");

    printf("SUCCESS: %d items verified (0 dropped, 0 duplicated, 0 corrupted).\n", TOTAL_ITEMS);
    return 0;
}