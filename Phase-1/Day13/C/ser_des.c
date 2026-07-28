#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Market tick data structure
typedef struct {
    uint32_t symbol_id; // 4 bytes
    uint64_t price;     // 8 bytes (fixed-point)
    uint32_t size;      // 4 bytes
    uint8_t  side;      // 1 byte (0=Buy, 1=Sell)
} Tick;

const size_t TICK_WIRE_SIZE = sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint8_t);

// Write uint32 big-endian
static inline void write_u32_be(uint8_t *buf, uint32_t val) {
    buf[0] = (uint8_t)(val >> 24);
    buf[1] = (uint8_t)(val >> 16);
    buf[2] = (uint8_t)(val >> 8);
    buf[3] = (uint8_t)(val);
}

// Write uint64 big-endian
static inline void write_u64_be(uint8_t *buf, uint64_t val) {
    buf[0] = (uint8_t)(val >> 56);
    buf[1] = (uint8_t)(val >> 48);
    buf[2] = (uint8_t)(val >> 40);
    buf[3] = (uint8_t)(val >> 32);
    buf[4] = (uint8_t)(val >> 24);
    buf[5] = (uint8_t)(val >> 16);
    buf[6] = (uint8_t)(val >> 8);
    buf[7] = (uint8_t)(val);
}

// Read uint32 big-endian
static inline uint32_t read_u32_be(const uint8_t *buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  |
            (uint32_t)buf[3];
}

// Read uint64 big-endian
static inline uint64_t read_u64_be(const uint8_t *buf) {
    return ((uint64_t)buf[0] << 56) |
           ((uint64_t)buf[1] << 48) |
           ((uint64_t)buf[2] << 40) |
           ((uint64_t)buf[3] << 32) |
           ((uint64_t)buf[4] << 24) |
           ((uint64_t)buf[5] << 16) |
           ((uint64_t)buf[6] << 8)  |
            (uint64_t)buf[7];
}

// Serialize struct -> byte stream
size_t serialize_tick(const Tick *src, uint8_t *buf, size_t buf_len) {
    size_t required_len = TICK_WIRE_SIZE; // 17 bytes
    if (buf_len < required_len) return 0;

    write_u32_be(buf + 0, src->symbol_id);
    write_u64_be(buf + 4, src->price);
    write_u32_be(buf + 12, src->size);
    buf[16] = src->side;

    return required_len;
}

// Deserialize byte stream -> struct
int deserialize_tick(const uint8_t *buf, size_t buf_len, Tick *dst) {
    size_t required_len = TICK_WIRE_SIZE;
    if (buf_len < required_len) return -1;

    dst->symbol_id = read_u32_be(buf + 0);
    dst->price     = read_u64_be(buf + 4);
    dst->size      = read_u32_be(buf + 12);
    dst->side      = buf[16];

    return 0;
}

int main(void) {
    Tick original = { .symbol_id = 1001, .price = 15025000, .size = 50, .side = 1 };
    uint8_t buffer[32];

    size_t bytes_written = serialize_tick(&original, buffer, sizeof(buffer));
    printf("Serialized size: %zu bytes\n", bytes_written);

    printf("Raw Bytes: ");
    for (size_t i = 0; i < bytes_written; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    Tick unpacked;
    if (deserialize_tick(buffer, bytes_written, &unpacked) == 0) {
        printf("Unpacked: ID=%u, Price=%llu, Size=%u, Side=%u\n",
               unpacked.symbol_id, (unsigned long long)unpacked.price, unpacked.size, unpacked.side);
    }

    return 0;
}