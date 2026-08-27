#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex mtx;

struct OrderBook {
    uint32_t bid_price{0};
    uint32_t ask_price{0};
    uint32_t volume{0};
    uint64_t quantity{0};
};

void update_book(OrderBook& book) {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> guard(mtx);
        book.bid_price = 100;
        book.ask_price = 105;
        book.volume += 1;
        book.quantity += 5;
    }
}

int main() {
    auto start = std::chrono::steady_clock::now();

    OrderBook book;

    std::thread t1(update_book, std::ref(book));
    std::thread t2(update_book, std::ref(book));

    t1.join();
    t2.join();

    std::cout << "Volume: " << book.volume << " (Expected 200000)\n";
    std::cout << "Quantity: " << book.quantity << " (Expected 1000000)\n";

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;


    return 0;
}