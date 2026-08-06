#include <iostream>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>
#include <thread>
#include <chrono>
#include <cmath>
#include <vector>

template <typename T, std::size_t Capacity>
class RingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity > 0, "Capacity must be greater than 0");

public:
    RingBuffer() : head_(0), tail_(0) {}

    template <typename... Args>
    bool try_emplace(Args&&... args) {
        const std::size_t current_tail = tail_.load(std::memory_order_relaxed);
        const std::size_t current_head = head_.load(std::memory_order_acquire);

        if ((current_tail - current_head) >= Capacity) {
            return false;
        }

        T* slot = reinterpret_cast<T*>(&buffer_[current_tail & mask_]);
        ::new (static_cast<void*>(slot)) T(std::forward<Args>(args)...);

        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    bool try_push(const T& item) {
        return try_emplace(item);
    }

    bool try_push(T&& item) {
        return try_emplace(std::move(item));
    }

    bool try_pop(T& value) {
        const std::size_t current_head = head_.load(std::memory_order_relaxed);
        const std::size_t current_tail = tail_.load(std::memory_order_acquire);

        if (current_head == current_tail) {
            return false;
        }

        T* slot = reinterpret_cast<T*>(&buffer_[current_head & mask_]);
        value = std::move(*slot);
        slot->~T();

        head_.store(current_head + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept {
        return Capacity;
    }

private:
    static constexpr std::size_t mask_ = Capacity - 1;

    alignas(64) std::atomic<std::size_t> head_;
    alignas(64) std::atomic<std::size_t> tail_;
    
    alignas(alignof(T)) std::aligned_storage_t<sizeof(T), alignof(T)> buffer_[Capacity];
};

constexpr std::size_t SAMPLE_RATE = 44100;
constexpr std::size_t BUFFER_SIZE = 8192;

// Real-time audio driver callback
// Zero allocs, zero locks, non-blocking
void audio_hardware_callback(float* out_buf, std::size_t frames, RingBuffer<float, BUFFER_SIZE>& ring) noexcept {
    for (std::size_t i = 0; i < frames; ++i) {
        float sample = 0.0f;
        if (!ring.try_pop(sample)) {
            // Underrun! Fill silence
            sample = 0.0f;
        }
        out_buf[i] = sample;
    }
}

// Background DSP thread generating 440Hz sine wave
void dsp_producer_thread(RingBuffer<float, BUFFER_SIZE>& ring, std::atomic<bool>& running) {
    float phase = 0.0f;
    const float phase_incr = 2.0f * 3.14159265358979323846f * 440.0f / static_cast<float>(SAMPLE_RATE);

    while (running.load(std::memory_order_relaxed)) {
        float sample = std::sin(phase);
        phase += phase_incr;
        if (phase >= 2.0f * 3.14159265358979323846f) {
            phase -= 2.0f * 3.14159265358979323846f;
        }

        while (!ring.try_push(sample) && running.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100)); // Sleep OK in background
        }
    }
}

int main() {
    auto* ring = new RingBuffer<float, BUFFER_SIZE>();
    std::atomic<bool> running{true};

    std::thread dsp_thread(dsp_producer_thread, std::ref(*ring), std::ref(running));

    std::vector<float> hardware_buffer(256);
    std::size_t total_frames_processed = 0;

    // Simulate hardware pulling 256-frame blocks every ~5ms for 2 seconds
    while (total_frames_processed < SAMPLE_RATE * 2) {
        audio_hardware_callback(hardware_buffer.data(), hardware_buffer.size(), *ring);
        total_frames_processed += hardware_buffer.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    running.store(false, std::memory_order_relaxed);
    dsp_thread.join();

    std::cout << "C++ Audio callback processed " << total_frames_processed << " frames without locks/allocs\n";

    delete ring;
    return 0;
}