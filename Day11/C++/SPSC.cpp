#include <iostream>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <optional>

template <typename T, std::size_t Capacity = 1024>
class SPSCQueue {
private:
    T data_[Capacity];
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};

public:
    SPSCQueue() = default;

    bool push(T val) {
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        std::size_t head = head_.load(std::memory_order_acquire);

        std::size_t next_tail = (tail + 1) % Capacity;
        if (next_tail == head) return false; // Full

        data_[tail] = std::move(val); // Write data FIRST
        tail_.store(next_tail, std::memory_order_release); // Publish AFTER write
        return true;
    }

    bool pop(T& val) {
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_acquire);

        if (head == tail) return false; // Empty

        val = std::move(data_[head]); // Read data FIRST
        std::size_t next_head = (head + 1) % Capacity;
        head_.store(next_head, std::memory_order_release); // Release slot AFTER read
        return true;
    }
};

int main() {
    SPSCQueue<uint64_t, 1024> q;

    if (q.push(1234)) {
        std::cout << "Pushed 1234\n";
    }

    uint64_t val = 0;
    if (q.pop(val)) {
        std::cout << "Popped " << val << "\n";
    }

    return 0;
}