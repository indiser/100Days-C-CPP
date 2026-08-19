#include <atomic>
#include <thread>
#include <chrono>
#include <cstdio>
#include <cstdlib>

constexpr size_t CACHE_LINE = 64;
constexpr long NUM_UPDATES = 1000000;

struct alignas(CACHE_LINE) Ticker {
    std::atomic<long> price{0};
    char pad1[CACHE_LINE - sizeof(std::atomic<long>)];
    std::atomic<long> volume{0};
    char pad2[CACHE_LINE - sizeof(std::atomic<long>)];
    std::atomic<long> seq{0};
    char pad3[CACHE_LINE - sizeof(std::atomic<long>)];
};

Ticker ticker;
std::atomic<bool> stop_flag{false};

void writer_thread() {
    long base_price = 10000;
    for (long i = 0; i < NUM_UPDATES; i++) {
        long new_price = base_price + (i % 500);
        long new_volume = (i * 7) % 1000;

        ticker.price.store(new_price, std::memory_order_relaxed);
        ticker.volume.store(new_volume, std::memory_order_relaxed);

        ticker.seq.store(i + 1, std::memory_order_release);
    }
    stop_flag.store(true, std::memory_order_release);
}

void reader_thread() {
    long last_seq = 0;
    long reads = 0;

    while (!stop_flag.load(std::memory_order_acquire)) {
        long seq = ticker.seq.load(std::memory_order_acquire);
        if (seq != last_seq) {
            [[maybe_unused]] long price = ticker.price.load(std::memory_order_relaxed);
            [[maybe_unused]] long volume = ticker.volume.load(std::memory_order_relaxed);
            last_seq = seq;
            reads++;
        }
    }

    std::printf("reader: saw %ld distinct updates\n", reads);
}

// CAS retry loop: update price only if higher than current
bool update_if_higher(std::atomic<long>& price, long candidate) {
    long current = price.load(std::memory_order_relaxed);
    while (candidate > current) {
        if (price.compare_exchange_weak(
                current, candidate,
                std::memory_order_release,
                std::memory_order_relaxed)) {
            return true;
        }
        // current auto-refreshed by compare_exchange_weak on failure
    }
    return false;
}

double bench_ordering(std::memory_order store_order, std::memory_order load_order, long iters) {
    std::atomic<long> counter{0};
    auto t0 = std::chrono::steady_clock::now();

    for (long i = 0; i < iters; i++) {
        counter.store(i, store_order);
        counter.load(load_order);
    }

    auto t1 = std::chrono::steady_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();
    return iters / secs;
}

int main() {
    std::thread w(writer_thread);
    std::thread r(reader_thread);

    w.join();
    r.join();

    // CAS demo
    std::atomic<long> best_price{0};
    long candidates[] = {100, 250, 90, 300, 300, 500};
    for (long c : candidates) {
        bool updated = update_if_higher(best_price, c);
        std::printf("candidate=%ld updated=%d best=%ld\n",
                     c, updated, best_price.load(std::memory_order_relaxed));
    }

    // ordering cost benchmark
    long bench_iters = 20000000;
    double seq_cst = bench_ordering(std::memory_order_seq_cst, std::memory_order_seq_cst, bench_iters);
    double rel_acq = bench_ordering(std::memory_order_release, std::memory_order_acquire, bench_iters);
    double relaxed = bench_ordering(std::memory_order_relaxed, std::memory_order_relaxed, bench_iters);

    std::printf("\nordering benchmark (%ld iters):\n", bench_iters);
    std::printf("seq_cst:         %.2f ops/sec\n", seq_cst);
    std::printf("release/acquire: %.2f ops/sec\n", rel_acq);
    std::printf("relaxed:         %.2f ops/sec\n", relaxed);

    return 0;
}