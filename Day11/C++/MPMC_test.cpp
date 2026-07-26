#include <iostream>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <thread>
#include <cassert>
#include <utility>

constexpr std::size_t CAPACITY = 1024;
constexpr std::size_t NUM_THREADS = 4;
constexpr std::size_t ITEMS_PER_THREAD = 100000;
constexpr std::size_t TOTAL_ITEMS = NUM_THREADS * ITEMS_PER_THREAD;

template <typename T, std::size_t Capacity = CAPACITY>
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

// Verification global atomics
std::atomic<uint8_t> received[TOTAL_ITEMS + 1]{0};
std::atomic<uint64_t> total_consumed_sum{0};

void producer(LockFreeQueue<uint64_t, CAPACITY>& q, std::size_t thread_id) {
    for (std::size_t i = 0; i < ITEMS_PER_THREAD; ++i) {
        uint64_t val = (thread_id * ITEMS_PER_THREAD) + i + 1;
        while (!q.push(val));
    }
}

void consumer(LockFreeQueue<uint64_t, CAPACITY>& q) {
    std::size_t popped_count = 0;
    uint64_t val = 0;

    while (popped_count < ITEMS_PER_THREAD) {
        if (q.pop(val)) {
            popped_count++;

            // Range validation
            assert(val >= 1 && val <= TOTAL_ITEMS);

            // Duplicate validation
            uint8_t prev = received[val].fetch_add(1, std::memory_order_relaxed);
            assert(prev == 0 && "DUPLICATE OR CORRUPTED VALUE RECEIVED!");

            total_consumed_sum.fetch_add(val, std::memory_order_relaxed);
        }
    }
}

int main() {
    LockFreeQueue<uint64_t, CAPACITY> q;

    uint64_t expected_sum = 0;
    for (uint64_t i = 1; i <= TOTAL_ITEMS; ++i) {
        expected_sum += i;
    }

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    producers.reserve(NUM_THREADS);
    consumers.reserve(NUM_THREADS);

    for (std::size_t i = 0; i < NUM_THREADS; ++i) {
        producers.emplace_back(producer, std::ref(q), i);
        consumers.emplace_back(consumer, std::ref(q));
    }

    for (size_t i = 0; i < NUM_THREADS; ++i) {
        producers[i].join();
        consumers[i].join();
    }

    // Verification step
    for (std::size_t i = 1; i <= TOTAL_ITEMS; ++i) {
        assert(received[i].load(std::memory_order_relaxed) == 1 && "MISSING ITEM DETECTED!");
    }
    assert(total_consumed_sum.load(std::memory_order_relaxed) == expected_sum && "SUM MISMATCH DETECTED!");

    std::cout << "SUCCESS: " << TOTAL_ITEMS 
              << " items verified (0 dropped, 0 duplicated, 0 corrupted).\n";

    return 0;
}