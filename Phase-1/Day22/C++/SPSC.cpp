#include <iostream>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <new>
#include <type_traits>
#include <utility>
#include <thread>
#include <cassert>
#include <limits>

template <typename T, std::size_t Capacity>
class RingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static_assert(Capacity > 0, "Capacity must be greater than 0");

public:
    RingBuffer() : head_(0), tail_(0) {}

    void seed_indices(std::size_t val) noexcept {
        head_.store(val, std::memory_order_relaxed);
        tail_.store(val, std::memory_order_relaxed);
    }

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

constexpr std::size_t NUM_OPS = 100000000ULL;

void producer(RingBuffer<std::uint64_t, 1024>& q) {
    for (std::uint64_t i = 0; i < NUM_OPS; ++i) {
        while (!q.try_push(i)) {
            // Spin
        }
    }
}

void consumer(RingBuffer<std::uint64_t, 1024>& q) {
    std::uint64_t val = 0;
    for (std::uint64_t i = 0; i < NUM_OPS; ++i) {
        while (!q.try_pop(val)) {
            // Spin
        }
        assert(val == i);
    }
}

int main() {
    auto* q = new RingBuffer<std::uint64_t, 1024>();

    std::size_t near_max = std::numeric_limits<std::size_t>::max() - 1000;
    q->seed_indices(near_max);

    std::thread prod(producer, std::ref(*q));
    std::thread cons(consumer, std::ref(*q));

    prod.join();
    cons.join();

    std::cout << "C++ RingBuffer passed " << NUM_OPS << " ops stress test + wraparound check\n";

    delete q;
    return 0;
}