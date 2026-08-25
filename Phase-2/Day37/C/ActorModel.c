#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<stdatomic.h>
#include<stdint.h>
#include<unistd.h>

#define CAPACITY 1024

typedef enum MessageType
{
    MSG_NEW_ORDER,
    MSG_CANCEL_ORDER,
    MSG_POISON_PILL
}MessageType;

typedef struct NewMessagePayload
{
    uint64_t order_id;
    uint32_t symbol;
    double price;
    uint32_t quantity;
    uint8_t side;
}NewMessagePayload;

typedef struct CancelMessagePayload
{
    uint64_t order_id;
    uint32_t symbol;
}CancelMessagePayload;

typedef struct
{
    MessageType type;
    union
    {
        NewMessagePayload *newOrder;
        CancelMessagePayload *cancelOrder;
    }payload;
}Message;


// Lock Free Queue
typedef struct
{
    uintptr_t data;
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

// Actor Model
typedef struct Actor
{
    pthread_t thread;
    LockFreeQueue *mailbox;
    void* state;
}Actor;

void *actor_worker(void *args)
{
    Actor *actor = (Actor*)args;
    uint64_t raw_msg;
    bool running = true;

    while (running)
    {
        if(!queue_pop(actor->mailbox, &raw_msg))
        {
            sched_yield();
            continue;
        }

        Message *msg = (Message*)(uintptr_t)raw_msg;
        switch (msg->type)
        {
            case MSG_NEW_ORDER:
                printf("[NEW ORDER] ID: %lu | SYMBOL: %u | PRICE: %.2f | QTY: %u | SIDE: %u\n", 
                        msg->payload.newOrder->order_id, 
                        msg->payload.newOrder->symbol, 
                        msg->payload.newOrder->price, 
                        msg->payload.newOrder->quantity, 
                        msg->payload.newOrder->side);
                free(msg->payload.newOrder);
                free(msg);
                break;
            case MSG_CANCEL_ORDER:
                printf("[CANCEL ORDER] ID: %lu | SYMBOL: %u\n", 
                        msg->payload.cancelOrder->order_id, 
                        msg->payload.cancelOrder->symbol);
                free(msg->payload.cancelOrder);
                free(msg);
                break;
            case MSG_POISON_PILL:
                printf("[ACTOR SHUTDOWN]\n");
                free(msg);
                running = false;
                break;
            default:
                break;
        }
    }
    return NULL;
}

int main()
{
    LockFreeQueue queue;
    initialize(&queue);

    Actor actor = { .mailbox = &queue, .state = NULL };
    pthread_create(&actor.thread, NULL, actor_worker, &actor);

    // Push NEW ORDER
    Message *msg1 = malloc(sizeof(Message));
    msg1->type = MSG_NEW_ORDER;
    msg1->payload.newOrder = malloc(sizeof(NewMessagePayload));
    msg1->payload.newOrder->order_id = 1001;
    msg1->payload.newOrder->symbol = 65; // 'A'
    msg1->payload.newOrder->price = 150.50;
    msg1->payload.newOrder->quantity = 10;
    msg1->payload.newOrder->side = 1; // BUY
    queue_push(&queue, (uintptr_t)msg1);

    // Push CANCEL ORDER
    Message *msg2 = malloc(sizeof(Message));
    msg2->type = MSG_CANCEL_ORDER;
    msg2->payload.cancelOrder = malloc(sizeof(CancelMessagePayload));
    msg2->payload.cancelOrder->order_id = 1001;
    msg2->payload.cancelOrder->symbol = 65;
    queue_push(&queue, (uintptr_t)msg2);

    // Push POISON PILL
    Message *msg3 = malloc(sizeof(Message));
    msg3->type = MSG_POISON_PILL;
    queue_push(&queue, (uintptr_t)msg3);

    pthread_join(actor.thread, NULL);
    return 0;
}