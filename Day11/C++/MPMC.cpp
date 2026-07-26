#include <iostream>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <utility>

template <typename T, std::size_t Capacity = 1024>
class LockFreeQueue {
private:
    struct Slot {
        T data;
        std::atomic<std::size_t> sequence;
    };

    Slot slots_[Capacity];
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};

public:
    LockFreeQueue() {
        for (std::size_t i = 0; i < Capacity; ++i) {
            slots_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool push(T val) {
        std::size_t pos = tail_.load(std::memory_order_relaxed);

        while (true) {
            Slot* slot = &slots_[pos % Capacity];
            std::size_t seq = slot->sequence.load(std::memory_order_acquire);
            auto diff = static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos);

            if (diff == 0) { // Ready to write
                if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    slot->data = std::move(val);
                    slot->sequence.store(pos + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // Queue full
            } else {
                pos = tail_.load(std::memory_order_relaxed);
            }
        }
    }

    bool pop(T& val) {
        std::size_t pos = head_.load(std::memory_order_relaxed);

        while (true) {
            Slot* slot = &slots_[pos % Capacity];
            std::size_t seq = slot->sequence.load(std::memory_order_acquire);
            auto diff = static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos + 1);

            if (diff == 0) { // Ready to read
                if (head_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed)) {
                    val = std::move(slot->data);
                    slot->sequence.store(pos + Capacity, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                return false; // Queue empty
            } else {
                pos = head_.load(std::memory_order_relaxed);
            }
        }
    }
};

int main() {
    LockFreeQueue<uint64_t, 1024> q;

    uint64_t val = 1234;

    if (q.push(val)) {
        std::cout << "Pushed " << val << "\n";
    } else {
        std::cout << "Push failed of " << val << "\n";
    }

    uint64_t val_pop = 0;
    if (q.pop(val_pop)) {
        std::cout << "Popped " << val_pop << "\n";
    } else {
        std::cout << "Pop failed of " << val_pop << "\n";
    }

    return 0;
}