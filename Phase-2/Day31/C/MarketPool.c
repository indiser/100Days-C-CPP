// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>

// #define NUM_WORKERS 4
// #define TOTAL_TICKS 12

// typedef struct {
//     char symbol[8];
//     double price;
//     int id;
// } Tick;

// Tick tick_queue[TOTAL_TICKS];
// int current_tick = 0;
// pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

// void* worker_func(void* arg) {
//     long worker_id = (long)arg;
    
//     while (1) {
//         Tick t;
//         int has_work = 0;

//         pthread_mutex_lock(&queue_mutex);
//         if (current_tick < TOTAL_TICKS) {
//             t = tick_queue[current_tick];
//             current_tick++;
//             has_work = 1;
//         }
//         pthread_mutex_unlock(&queue_mutex);

//         if (!has_work) break;

//         printf("Worker %ld processed tick %d: %s @ $%.2f\n", 
//                worker_id, t.id, t.symbol, t.price);
//         usleep(100000); 
//     }
    
//     return NULL;
// }

// int main(void) {
//     pthread_t workers[NUM_WORKERS];

//     for (int i = 0; i < TOTAL_TICKS; i++) {
//         snprintf(tick_queue[i].symbol, 8, "AAPL");
//         tick_queue[i].price = 150.0 + i;
//         tick_queue[i].id = i + 1;
//     }

//     for (long i = 0; i < NUM_WORKERS; i++) {
//         if (pthread_create(&workers[i], NULL, worker_func, (void*)i) != 0) {
//             perror("pthread_create failed");
//             return 1;
//         }
//     }

//     for (int i = 0; i < NUM_WORKERS; i++) {
//         pthread_join(workers[i], NULL);
//     }

//     pthread_mutex_destroy(&queue_mutex);
//     printf("All ticks processed. Exit.\n");
//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_WORKERS 4
#define TOTAL_TICKS 12

typedef struct {
    char symbol[8];
    double price;
    int id;
} Tick;

static Tick tick_queue[TOTAL_TICKS];
static int current_tick = 0;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void* worker_func(void* arg) {
    long worker_id = (long)arg;
    
    while (1) {
        Tick t;
        int has_work = 0;

        if (pthread_mutex_lock(&queue_mutex) != 0) {
            perror("pthread_mutex_lock failed");
            pthread_exit((void*)1);
        }

        if (current_tick < TOTAL_TICKS) {
            t = tick_queue[current_tick++];
            has_work = 1;
        }

        if (pthread_mutex_unlock(&queue_mutex) != 0) {
            perror("pthread_mutex_unlock failed");
            pthread_exit((void*)1);
        }

        if (!has_work) break;

        printf("Worker %ld processed tick %d: %s @ $%.2f\n", 
               worker_id, t.id, t.symbol, t.price);
    }
    
    return NULL;
}

int main(void) {
    pthread_t workers[NUM_WORKERS];
    int spawned = 0;
    int ret = 0;

    for (int i = 0; i < TOTAL_TICKS; i++) {
        snprintf(tick_queue[i].symbol, sizeof(tick_queue[i].symbol), "AAPL");
        tick_queue[i].price = 150.0 + i;
        tick_queue[i].id = i + 1;
    }

    for (long i = 0; i < NUM_WORKERS; i++) {
        if (pthread_create(&workers[i], NULL, worker_func, (void*)i) != 0) {
            perror("pthread_create failed");
            ret = 1;
            break;
        }
        spawned++;
    }

    for (int i = 0; i < spawned; i++) {
        void* retval;
        if (pthread_join(workers[i], &retval) != 0) {
            perror("pthread_join failed");
            ret = 1;
        } else if (retval != NULL) {
            fprintf(stderr, "Worker thread %d exited with error\n", i);
            ret = 1;
        }
    }

    if (pthread_mutex_destroy(&queue_mutex) != 0) {
        perror("pthread_mutex_destroy failed");
        ret = 1;
    }

    if (ret == 0) {
        printf("All ticks processed cleanly. Exit.\n");
    }
    
    return ret;
}