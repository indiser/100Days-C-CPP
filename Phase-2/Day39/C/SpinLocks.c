#include<stdio.h>
#include<stdatomic.h>
#include<stdint.h>
#include<pthread.h>
#include<time.h>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #define cpu_relax() _mm_pause()
#else
    #define cpu_relax() ((void)0)
#endif

typedef struct 
{
    atomic_flag flag;
}spinlock_t;

#define SPINLOCK_INIT { ATOMIC_FLAG_INIT }

void spinLock_init(spinlock_t *spinLock)
{
    atomic_flag_clear(&spinLock->flag);
}

void acquire(spinlock_t *spinLock)
{
    while(atomic_flag_test_and_set_explicit(&spinLock->flag, memory_order_acquire))
    {
        cpu_relax();
    }
}

void release(spinlock_t *spinLock)
{
    atomic_flag_clear_explicit(&spinLock->flag, memory_order_release);
}

typedef struct
{
    uint32_t bid_price;
    uint32_t price;
    uint32_t volume;
    uint64_t quantity;
}OrderBooks;

typedef struct {
    OrderBooks *book;
    uint32_t ask_bid;
    uint32_t ask_price;
    uint32_t ask_volume;
    uint64_t ask_quantity;
    spinlock_t *lock;
} ThreadArgs;


void *updateOrder(void *args)
{
    ThreadArgs *targs = (ThreadArgs*)args;
    for (int i = 0; i < 100000; i++)
    {
        acquire(targs->lock);
        targs->book->bid_price = targs->ask_bid;
        targs->book->price = targs->ask_price;
        targs->book->volume += targs->ask_volume;
        targs->book->quantity += targs->ask_quantity;
        release(targs->lock);
    }
    return NULL;
}

int main()
{
    clock_t begin = clock();
    pthread_t p1, p2, p3, p4;
    spinlock_t lock = SPINLOCK_INIT;
    spinLock_init(&lock);

    OrderBooks books = {0};

    ThreadArgs args = {
        .book = &books,
        .ask_bid = 100,
        .ask_price = 105,
        .ask_volume = 1,
        .ask_quantity = 5,
        .lock = &lock
    };

    pthread_create(&p1, NULL, updateOrder, &args);
    pthread_create(&p2, NULL, updateOrder, &args);
    pthread_create(&p3, NULL, updateOrder, &args);
    pthread_create(&p4, NULL, updateOrder, &args);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);

    printf("Final Volume (Expected 400000): %u\n", books.volume);
    printf("Final Quantity (Expected 2000000): %lu\n", books.quantity);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    
    printf("Execution time: %.2f\n", time_spent);

    return 0;
}