#include <cstddef>
#include <cstdint>
#include <span>
#include <optional>

struct PacketCPP {
    uint8_t type;
    uint16_t length;
    std::span<const uint8_t> value;
};

// Safe property-based parser
std::optional<PacketCPP> parse_tlv_cpp(std::span<const uint8_t> data) {
    if (data.size() < 3) return std::nullopt;

    uint8_t type = data[0];
    uint16_t length = static_cast<uint16_t>((data[1] << 8) | data[2]);

    // Property check: payload must fit in remaining span
    if (data.size() - 3 < length) return std::nullopt;

    return PacketCPP{
        .type = type,
        .length = length,
        .value = data.subspan(3, length)
    };
}