// Multi Producer Multi Consumer using Dmitry Vyukov MPMC bounded Queue design with slots
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<stdatomic.h>

#define CAPACITY 1024

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


int main()
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