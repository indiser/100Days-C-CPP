#include <iostream>
#include <cstdint>
#include <cstddef>
#include <new>
using namespace std;

typedef struct alignas(64) MarketTick {
    uint64_t timestamp;
    double price;
    uint64_t qty;
    uint64_t flags;
    char symbols[8];
} Tick;

int main() {
    Tick tick;
    static_assert(alignof(Tick) == 64, "Not aligned"); //Only when aligned to 64 no error

    cout <<"Size: " << sizeof(tick) << endl;
    cout <<"Align of: " << alignof(tick) << endl;
    cout <<"Addr aligned to 64: " <<(((uintptr_t)&tick % 64 == 0) ? "YES" : "NO") << endl;
    cout <<"Address of timestamp: "<< &tick.timestamp << endl;
    cout <<"Address of price: "<< &tick.price << endl;
    cout <<"Address of quality: " << &tick.qty << endl;
    cout <<"Address of flags: " << &tick.flags << endl;
    cout <<"Address of symbols: " << &tick.symbols << endl;

    cout <<"OFFSET of timestamp: "<< offsetof(struct MarketTick, timestamp) << endl;
    cout <<"OFFSET of price: " << offsetof(struct MarketTick, price) << endl;
    cout <<"OFFSET of quality: "<< offsetof(struct MarketTick, qty) << endl;
    cout <<"OFFSET of flags: "<< offsetof(struct MarketTick, flags) << endl;
    cout <<"OFFSET of symbols: "<< offsetof(struct MarketTick, symbols) << endl;

    return 0;
}