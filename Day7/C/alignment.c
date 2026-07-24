#include<stdio.h>
#include<stdint.h>
#include<stdalign.h>
#include<stddef.h>
#include<assert.h>

#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))

typedef struct __attribute__((aligned(64))) MarketTick
{
    uint64_t timestamp;
    double price;
    uint64_t qty;
    uint64_t flags;
    char symbols[8];
} Tick;


int main()
{
    Tick tick;
    
    static_assert(alignof(Tick) == 64, "Not aligned"); //Only when aligned to 64 no error

    printf("Size: %zu\n", sizeof(tick));
    printf("Align of: %zu\n", alignof(tick));
    printf("Addr aligned to 64: %s\n", ((uintptr_t)&tick % 64 == 0) ? "YES" : "NO");
    printf("Address of timestamp: %p\n", &tick.timestamp);
    printf("Address of price: %p\n", &tick.price);
    printf("Address of quality: %p\n", &tick.qty);
    printf("Address of flags: %p\n", &tick.flags);
    printf("Address of symbols: %p\n", &tick.symbols);

    printf("OFFSET of timestamp: %lu\n", offsetof(struct MarketTick, timestamp));
    printf("OFFSET of price: %lu\n", OFFSETOF(struct MarketTick, price));
    printf("OFFSET of quality: %lu\n", OFFSETOF(struct MarketTick, qty));
    printf("OFFSET of flags: %lu\n", OFFSETOF(struct MarketTick, flags));
    printf("OFFSET of symbols: %lu\n", OFFSETOF(struct MarketTick, symbols));

    return 0;
}