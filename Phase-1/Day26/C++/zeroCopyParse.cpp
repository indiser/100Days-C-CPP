#include <iostream>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <vector>
#include <functional>

#if defined(_MSC_VER)
    #include <stdlib.h>
    #define BSWAP32(x) _byteswap_ulong(x)
    #define BSWAP64(x) _byteswap_uint64(x)
#else
    #define BSWAP32(x) __builtin_bswap32(x)
    #define BSWAP64(x) __builtin_bswap64(x)
#endif

struct MarketTickView {
    std::string_view symbol; // Non-owning view into raw payload
    uint32_t price;
    uint64_t quantity;
};

class FeedParser {
public:
    static constexpr size_t TICK_SIZE = 16;

    // Safe, unaligned endian conversion
    static uint32_t read_be32(const char* ptr) {
        uint32_t val;
        std::memcpy(&val, ptr, sizeof(val));
        return BSWAP32(val);
    }

    static uint64_t read_be64(const char* ptr) {
        uint64_t val;
        std::memcpy(&val, ptr, sizeof(val));
        return BSWAP64(val);
    }

    // Process stream view. Returns consumed byte count.
    static size_t parse_feed(std::string_view stream, 
                            const std::function<void(const MarketTickView&)>& callback) {
        size_t offset = 0;

        while (offset + TICK_SIZE <= stream.size()) {
            const char* tick_ptr = stream.data() + offset;

            MarketTickView tick{
                .symbol   = std::string_view(tick_ptr, 4), // Zero-copy view creation
                .price    = read_be32(tick_ptr + 4),
                .quantity = read_be64(tick_ptr + 8)
            };

            callback(tick);
            offset += TICK_SIZE;
        }

        return offset; // Consumed bytes count
    }
};

int main() {
    const uint8_t stream_bytes[37] = {
        'A','A','P','L', 0x00,0x00,0x00,0x64, 0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE8,
        'M','S','F','T', 0x00,0x00,0x01,0x90, 0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF4,
        'N','V','D','A', 0x00
    };

    std::string_view feed_buffer(reinterpret_cast<const char*>(stream_bytes), sizeof(stream_bytes));

    auto handle_tick = [](const MarketTickView& tick) {
        std::cout << "Tick -> Symbol: " << tick.symbol 
                  << " | Price: " << tick.price 
                  << " | Qty: " << tick.quantity << "\n";
    };

    std::cout << "--- Processing C++ Feed Stream ---\n";
    size_t consumed = FeedParser::parse_feed(feed_buffer, handle_tick);

    std::cout << "\nStream size: " << feed_buffer.size() << " bytes\n";
    std::cout << "Consumed   : " << consumed << " bytes\n";
    std::cout << "Unconsumed : " << feed_buffer.size() - consumed << " bytes\n";

    return 0;
}