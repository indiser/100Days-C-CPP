#include <cstdio>
#include <cstdint>
#include <cassert>
#include <vector>
#include <cinttypes>
#include <type_traits>

struct Tick {
    uint32_t symbol_id;
    uint64_t price;
    uint32_t size;
    uint8_t  side;
};

constexpr size_t TICK_WIRE_SIZE = sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint8_t);

// kept exactly as-is
void write_be32(uint8_t* buf, uint32_t val) {
    buf[0] = val >> 24; buf[1] = val >> 16; buf[2] = val >> 8; buf[3] = val;
}
void write_be64(uint8_t* buf, uint64_t val) {
    for (int i = 0; i < 8; i++) buf[i] = val >> (56 - 8 * i);
}
uint32_t read_be32(const uint8_t* buf) {
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}
uint64_t read_be64(const uint8_t* buf) {
    uint64_t val = 0;
    for (int i = 0; i < 8; i++) val = (val << 8) | buf[i];
    return val;
}

// template dispatch layer — picks right concrete fn based on T
template <typename T>
void write_be(uint8_t* buf, T val) {
    if constexpr (sizeof(T) == 4) write_be32(buf, static_cast<uint32_t>(val));
    else if constexpr (sizeof(T) == 8) write_be64(buf, static_cast<uint64_t>(val));
    else static_assert(sizeof(T) == 4 || sizeof(T) == 8, "unsupported width");
}

template <typename T>
T read_be(const uint8_t* buf) {
    if constexpr (sizeof(T) == 4) return static_cast<T>(read_be32(buf));
    else if constexpr (sizeof(T) == 8) return static_cast<T>(read_be64(buf));
    else static_assert(sizeof(T) == 4 || sizeof(T) == 8, "unsupported width");
}

// templated serializer, works on std::vector<uint8_t>
class TickSerializer {
public:
    static std::vector<uint8_t> serialize(const Tick& t) {
        std::vector<uint8_t> buf(TICK_WIRE_SIZE);
        size_t off = 0;
        write_be<uint32_t>(buf.data() + off, t.symbol_id); off += sizeof(uint32_t);
        write_be<uint64_t>(buf.data() + off, t.price);      off += sizeof(uint64_t);
        write_be<uint32_t>(buf.data() + off, t.size);        off += sizeof(uint32_t);
        buf[off] = t.side;                                    off += sizeof(uint8_t);
        return buf;
    }

    static bool deserialize(const std::vector<uint8_t>& buf, Tick& t) {
        if (buf.size() < TICK_WIRE_SIZE) return false;
        size_t off = 0;
        t.symbol_id = read_be<uint32_t>(buf.data() + off); off += sizeof(uint32_t);
        t.price     = read_be<uint64_t>(buf.data() + off); off += sizeof(uint64_t);
        t.size      = read_be<uint32_t>(buf.data() + off); off += sizeof(uint32_t);
        t.side      = buf[off];                              off += sizeof(uint8_t);
        return true;
    }
};

int main() {
    Tick original{1001, 15025000, 50, 1};

    std::vector<uint8_t> buffer = TickSerializer::serialize(original);
    printf("Serialized size: %zu bytes\n", buffer.size());

    printf("Raw Bytes: ");
    for (auto b : buffer) printf("%02X ", b);
    printf("\n");

    Tick unpacked{};
    bool ok = TickSerializer::deserialize(buffer, unpacked);
    assert(ok);
    assert(unpacked.symbol_id == original.symbol_id);
    assert(unpacked.price == original.price);
    assert(unpacked.size == original.size);
    assert(unpacked.side == original.side);

    printf("Unpacked: ID=%u, Price=%" PRIu64 ", Size=%u, Side=%u\n",
           unpacked.symbol_id, unpacked.price, unpacked.size, unpacked.side);
    printf("Round-trip OK\n");
    return 0;
}