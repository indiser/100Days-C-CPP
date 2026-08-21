#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    void (*func)(void*);
    void *args;
} task_t;

typedef struct {
    task_t *tasks;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    int shutdown;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    size_t thread_count;
} threadpool_t;

static void *worker_thread(void *arg) {
    threadpool_t *pool = (threadpool_t *)arg;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }
        if (pool->shutdown && pool->count == 0) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }
        
        /* Copy struct to local stack to drop lock fast, prevent worker holding lock during work */
        task_t task = pool->tasks[pool->head];
        pool->head = (pool->head + 1) % pool->capacity;
        pool->count--;
        pthread_mutex_unlock(&pool->lock);

        task.func(task.args);
    }
    return NULL;
}

void threadpool_destroy(threadpool_t *pool);

threadpool_t *threadpool_create(size_t thread_count, size_t capacity) {
    if (thread_count == 0 || capacity == 0) return NULL;

    threadpool_t *pool = malloc(sizeof(threadpool_t));
    if (!pool) return NULL;

    pool->capacity = capacity;
    pool->head = 0;
    pool->tail = 0;
    pool->count = 0;
    pool->shutdown = 0;
    pool->thread_count = 0;

    pool->tasks = malloc(sizeof(task_t) * capacity);
    pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (!pool->tasks || !pool->threads) {
        free(pool->tasks);
        free(pool->threads);
        free(pool);
        return NULL;
    }

    if (pthread_mutex_init(&pool->lock, NULL) != 0 ||
        pthread_cond_init(&pool->notify, NULL) != 0) {
        free(pool->tasks);
        free(pool->threads);
        free(pool);
        return NULL;
    }

    for (size_t i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            /* Cleanup already created threads */
            threadpool_destroy(pool);
            return NULL;
        }
        pool->thread_count++;
    }

    return pool;
}

int threadpool_add(threadpool_t *pool, void (*func)(void *), void *args) {
    if (!pool || !func) return -1;

    pthread_mutex_lock(&pool->lock);
    if (pool->count == pool->capacity || pool->shutdown) {
        pthread_mutex_unlock(&pool->lock);
        return -1;
    }

    pool->tasks[pool->tail].func = func;
    pool->tasks[pool->tail].args = args;
    pool->tail = (pool->tail + 1) % pool->capacity;
    pool->count++;
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    return 0;
}

void threadpool_destroy(threadpool_t *pool) {
    if (!pool) return;

    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    for (size_t i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    free(pool->tasks);
    free(pool->threads);
    free(pool);
}

void sample_work(void *arg) {
    int id = *(int *)arg;
    printf("Task %d done\n", id);
    free(arg);
}

int main() {
    threadpool_t *pool = threadpool_create(4, 16);
    if (!pool) {
        fprintf(stderr, "Failed to create pool\n");
        return 1;
    }

    for (int i = 0; i < 8; i++) {
        int *val = malloc(sizeof(int));
        if (!val) continue;
        *val = i;

        if (threadpool_add(pool, sample_work, val) != 0) {
            fprintf(stderr, "Queue full, task %d dropped\n", i);
            free(val);
        }
    }

    /* No sleep needed, destroy joins all worker threads after task completion */
    threadpool_destroy(pool);
    return 0;
}