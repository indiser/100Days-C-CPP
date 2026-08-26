#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define FRAME_SIZE 256
#define QUEUE_CAP 4

typedef struct {
    float data[FRAME_SIZE];
    bool is_last;
} Frame;

typedef struct {
    Frame buffer[QUEUE_CAP];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} AudioQueue;

typedef struct {
    AudioQueue *out_queue;
    int total_frames;
} ProducerArgs;

typedef struct {
    AudioQueue *in_queue;
    AudioQueue *out_queue;
    float gain;
} FilterArgs;

typedef struct {
    AudioQueue *in_queue;
} ConsumerArgs;

// Error check helper
#define CHECK_PTHREAD(cmd) \
    do { \
        int err = (cmd); \
        if (err != 0) { \
            fprintf(stderr, "FATAL: %s failed at %s:%d - %s\n", \
                    #cmd, __FILE__, __LINE__, strerror(err)); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

void queue_init(AudioQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    CHECK_PTHREAD(pthread_mutex_init(&q->lock, NULL));
    CHECK_PTHREAD(pthread_cond_init(&q->not_full, NULL));
    CHECK_PTHREAD(pthread_cond_init(&q->not_empty, NULL));
}

void queue_destroy(AudioQueue *q) {
    CHECK_PTHREAD(pthread_mutex_destroy(&q->lock));
    CHECK_PTHREAD(pthread_cond_destroy(&q->not_full));
    CHECK_PTHREAD(pthread_cond_destroy(&q->not_empty));
}

void queue_push(AudioQueue *q, const Frame *frame) {
    CHECK_PTHREAD(pthread_mutex_lock(&q->lock));
    while (q->count == QUEUE_CAP) {
        CHECK_PTHREAD(pthread_cond_wait(&q->not_full, &q->lock));
    }
    q->buffer[q->tail] = *frame;
    q->tail = (q->tail + 1) % QUEUE_CAP;
    q->count++;
    CHECK_PTHREAD(pthread_cond_signal(&q->not_empty));
    CHECK_PTHREAD(pthread_mutex_unlock(&q->lock));
}

Frame queue_pop(AudioQueue *q) {
    CHECK_PTHREAD(pthread_mutex_lock(&q->lock));
    while (q->count == 0) {
        CHECK_PTHREAD(pthread_cond_wait(&q->not_empty, &q->lock));
    }
    Frame frame = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_CAP;
    q->count--;
    CHECK_PTHREAD(pthread_cond_signal(&q->not_full));
    CHECK_PTHREAD(pthread_mutex_unlock(&q->lock));
    return frame;
}

void *stage1_producer(void *arg) {
    ProducerArgs *pargs = (ProducerArgs *)arg;
    float phase = 0.0f;

    for (int i = 0; i < pargs->total_frames; i++) {
        Frame f;
        f.is_last = false;
        for (int s = 0; s < FRAME_SIZE; s++) {
            f.data[s] = sinf(phase);
            phase += 0.05f;
        }
        queue_push(pargs->out_queue, &f);
    }

    Frame poison_pill = { .is_last = true };
    queue_push(pargs->out_queue, &poison_pill);
    return NULL;
}

void *stage2_filter(void *arg) {
    FilterArgs *fargs = (FilterArgs *)arg;
    while (1) {
        Frame f = queue_pop(fargs->in_queue);
        if (f.is_last) {
            queue_push(fargs->out_queue, &f);
            break;
        }
        for (int s = 0; s < FRAME_SIZE; s++) {
            f.data[s] *= fargs->gain;
        }
        queue_push(fargs->out_queue, &f);
    }
    return NULL;
}

void *stage3_consumer(void *arg) {
    ConsumerArgs *cargs = (ConsumerArgs *)arg;
    int count = 0;
    while (1) {
        Frame f = queue_pop(cargs->in_queue);
        if (f.is_last) break;
        count++;
    }
    printf("Successfully consumed %d frames through pipeline.\n", count);
    return NULL;
}

int main() {
    AudioQueue q1, q2;
    queue_init(&q1);
    queue_init(&q2);

    // Stress Test: Capacity=4, Frames=100000 (forces heavy contention & wrap)
    ProducerArgs p_args = { .out_queue = &q1, .total_frames = 100000 };
    FilterArgs   f_args = { .in_queue = &q1, .out_queue = &q2, .gain = 1.5f };
    ConsumerArgs c_args = { .in_queue = &q2 };

    pthread_t t1, t2, t3;

    CHECK_PTHREAD(pthread_create(&t1, NULL, stage1_producer, &p_args));
    CHECK_PTHREAD(pthread_create(&t2, NULL, stage2_filter, &f_args));
    CHECK_PTHREAD(pthread_create(&t3, NULL, stage3_consumer, &c_args));

    CHECK_PTHREAD(pthread_join(t1, NULL));
    CHECK_PTHREAD(pthread_join(t2, NULL));
    CHECK_PTHREAD(pthread_join(t3, NULL));

    queue_destroy(&q1);
    queue_destroy(&q2);

    printf("Pipeline completed cleanly.\n");
    return 0;
}