#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#if defined(_MSC_VER)
    #include <stdlib.h>
    #define BSWAP32(x) _byteswap_ulong(x)
    #define BSWAP64(x) _byteswap_uint64(x)
#else
    #define BSWAP32(x) __builtin_bswap32(x)
    #define BSWAP64(x) __builtin_bswap64(x)
#endif

#define TICK_SIZE 16

typedef struct {
    const char *symbol;
    size_t sym_len;
    uint32_t price;
    uint64_t quantity;
} MarketTickView;

static inline uint32_t read_be32(const uint8_t *ptr) {
    uint32_t val;
    memcpy(&val, ptr, sizeof(val));
    return BSWAP32(val);
}

static inline uint64_t read_be64(const uint8_t *ptr) {
    uint64_t val;
    memcpy(&val, ptr, sizeof(val));
    return BSWAP64(val);
}

size_t parse_feed(const uint8_t *buf, size_t len, void (*on_tick)(const MarketTickView *)) {
    size_t offset = 0;

    while (offset + TICK_SIZE <= len) {
        const uint8_t *tick_ptr = buf + offset;
        
        MarketTickView tick = {
            .symbol  = (const char *)tick_ptr,
            .sym_len = 4,
            .price   = read_be32(tick_ptr + 4),
            .quantity= read_be64(tick_ptr + 8)
        };

        on_tick(&tick);
        offset += TICK_SIZE;
    }

    return offset; // Return consumed byte count
}

static void handle_tick(const MarketTickView *tick) {
    printf("Tick -> Symbol: %.4s, Price: %u, Qty: %llu\n", 
           tick->symbol, tick->price, (unsigned long long)tick->quantity);
}


int main(void) {
    uint8_t stream[37] = {
        'A','A','P','L', 0x00,0x00,0x00,0x64, 0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE8,
        'M','S','F','T', 0x00,0x00,0x01,0x90, 0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF4,
        'N','V','D','A', 0x00
    };

    printf("--- Processing Feed Stream ---\n");
    size_t consumed = parse_feed(stream, sizeof(stream), handle_tick);

    printf("\nStream size: %zu bytes\n", sizeof(stream));
    printf("Consumed   : %zu bytes\n", consumed);
    printf("Unconsumed : %zu bytes (save to ring buffer for next packet)\n", 
           sizeof(stream) - consumed);

    return 0;
}