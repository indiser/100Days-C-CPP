#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <array>

constexpr uint64_t ITERS = 200000000ULL;

// BAD: no padding, adjacent counters share cache line
struct TickBad {
    uint64_t counter;
};

// GOOD: forced onto own cache line
struct alignas(64) TickGood {
    uint64_t counter;
};

std::array<TickBad, 2>  bad_arr{};
std::array<TickGood, 2> good_arr{};

void bump_bad(int idx) {
    for (uint64_t i = 0; i < ITERS; i++) {
        bad_arr[idx].counter++;
    }
}

void bump_good(int idx) {
    for (uint64_t i = 0; i < ITERS; i++) {
        good_arr[idx].counter++;
    }
}

template <typename Fn>
double run(Fn fn) {
    auto start = std::chrono::steady_clock::now();

    std::thread t0(fn, 0);
    std::thread t1(fn, 1);
    t0.join();
    t1.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    std::cout << "sizeof(TickBad)  = " << sizeof(TickBad)  << ", alignof = " << alignof(TickBad)  << "\n";
    std::cout << "sizeof(TickGood) = " << sizeof(TickGood) << ", alignof = " << alignof(TickGood) << "\n";

    std::cout << "bad_arr[0] addr: " << &bad_arr[0] << ", bad_arr[1] addr: " << &bad_arr[1]
               << ", diff: " << (reinterpret_cast<char*>(&bad_arr[1]) - reinterpret_cast<char*>(&bad_arr[0])) << "\n";
    std::cout << "good_arr[0] addr: " << &good_arr[0] << ", good_arr[1] addr: " << &good_arr[1]
               << ", diff: " << (reinterpret_cast<char*>(&good_arr[1]) - reinterpret_cast<char*>(&good_arr[0])) << "\n\n";

    double t_bad = run(bump_bad);
    std::cout << "False-sharing (unaligned) time: " << t_bad << " sec\n";

    double t_good = run(bump_good);
    std::cout << "Cache-aligned (no false-sharing) time: " << t_good << " sec\n";

    std::cout << "\nSlowdown factor from false sharing: " << (t_bad / t_good) << "x\n";

    return 0;
}