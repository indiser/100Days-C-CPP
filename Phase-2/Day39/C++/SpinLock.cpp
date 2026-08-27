#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #define cpu_relax() _mm_pause()
#else
    #define cpu_relax() ((void)0)
#endif

// Custom Spinlock Primitive
class Spinlock {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() noexcept {
        while (flag.test_and_set(std::memory_order_acquire)) {
            cpu_relax();
        }
    }

    void unlock() noexcept {
        flag.clear(std::memory_order_release);
    }
};

// Custom RAII Guard (std::lock_guard clone)
class LockGuard {
private:
    Spinlock& lock_;

public:
    explicit LockGuard(Spinlock& lock) : lock_(lock) {
        lock_.lock();
    }
    
    ~LockGuard() {
        lock_.unlock();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};

struct OrderBook {
    uint32_t bid_price{0};
    uint32_t ask_price{0};
    uint32_t volume{0};
    uint64_t quantity{0};
};

void update_book(OrderBook& book, Spinlock& spinlock) {
    for (int i = 0; i < 100000; ++i) {
        LockGuard guard(spinlock);
        book.bid_price = 100;
        book.ask_price = 105;
        book.volume += 1;
        book.quantity += 5;
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();

    OrderBook book;
    Spinlock spinlock;

    std::thread t1(update_book, std::ref(book), std::ref(spinlock));
    std::thread t2(update_book, std::ref(book), std::ref(spinlock));

    t1.join();
    t2.join();

    std::cout << "Volume: " << book.volume << " (Expected 200000)\n";
    std::cout << "Quantity: " << book.quantity << " (Expected 1000000)\n";

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;


    return 0;
}