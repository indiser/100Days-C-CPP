#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdalign.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define CAPACITY 8192
#define MASK (CAPACITY - 1)

_Static_assert((CAPACITY & (CAPACITY - 1)) == 0, "CAPACITY must be power of 2");

typedef struct {
    float data[CAPACITY];
    alignas(64) _Atomic size_t head;
    alignas(64) _Atomic size_t tail;
} AudioRingBuffer;

void ring_init(AudioRingBuffer *q) {
    atomic_store_explicit(&q->head, 0, memory_order_relaxed);
    atomic_store_explicit(&q->tail, 0, memory_order_relaxed);
}

bool ring_push(AudioRingBuffer *q, float sample) {
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    size_t current_head = atomic_load_explicit(&q->head, memory_order_acquire);

    if ((current_tail - current_head) >= CAPACITY) {
        return false;
    }

    q->data[current_tail & MASK] = sample;
    atomic_store_explicit(&q->tail, current_tail + 1, memory_order_release);
    return true;
}

bool ring_pop(AudioRingBuffer *q, float *sample) {
    size_t current_head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t current_tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if (current_head == current_tail) {
        return false;
    }

    *sample = q->data[current_head & MASK];
    atomic_store_explicit(&q->head, current_head + 1, memory_order_release);
    return true;
}

// Simulated real-time audio driver callback (e.g., ALSA / PortAudio)
// Zero allocs, zero locks, non-blocking
void audio_hardware_callback(float *out_buf, size_t frames, AudioRingBuffer *ring) {
    for (size_t i = 0; i < frames; ++i) {
        float sample = 0.0f;
        if (!ring_pop(ring, &sample)) {
            // Underrun! Fill silence, never block driver thread
            sample = 0.0f;
        }
        out_buf[i] = sample;
    }
}

// Background producer thread (DSP / Sine generator)
void* dsp_producer_thread(void *arg) {
    AudioRingBuffer *ring = (AudioRingBuffer*)arg;
    float phase = 0.0f;
    float phase_incr = 2.0f * 3.14159265358979323846f * 440.0f / SAMPLE_RATE; // 440 Hz A note

    for (size_t i = 0; i < SAMPLE_RATE * 2; ++i) { // 2 seconds audio
        float sample = sinf(phase);
        phase += phase_incr;
        if (phase >= 2.0f * 3.14159265358979323846f) {
            phase -= 2.0f * 3.14159265358979323846f;
        }

        while (!ring_push(ring, sample)) {
            usleep(100); // Sleep allowed on producer, NOT on callback
        }
    }
    return NULL;
}

int main(void) {
    AudioRingBuffer *ring = malloc(sizeof(AudioRingBuffer));
    ring_init(ring);

    pthread_t prod;
    pthread_create(&prod, NULL, dsp_producer_thread, ring);

    // Simulate audio driver pulling 256-frame blocks
    float out_buffer[256];
    size_t total_frames_processed = 0;

    while (total_frames_processed < SAMPLE_RATE * 2) {
        audio_hardware_callback(out_buffer, 256, ring);
        total_frames_processed += 256;
        usleep(5000); // Simulate 5ms hardware buffer interval
    }

    pthread_join(prod, NULL);
    printf("Audio callback processed %zu frames without lock/alloc\n", total_frames_processed);

    free(ring);
    return 0;
}