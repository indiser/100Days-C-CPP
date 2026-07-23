#include "parser.h"

int parse_tlv(const uint8_t *data, size_t size, Packet *out) {
    // Header must have at least Type (1 byte) + Length (2 bytes) = 3 bytes
    if (size < 3) return -1;

    out->type = data[0];
    // Big-endian uint16 length
    out->length = (uint16_t)((data[1] << 8) | data[2]);

    // Bug trap for fuzzer: payload check missing proper bounds or overflow
    if (size - 3 < out->length) return -1;

    out->value = &data[3];
    return 0;
}

// Buffer Overflow bug
// #include "parser.h"

// int parse_tlv(const uint8_t *data, size_t size, Packet *out) {
//     if (size < 3) return -1;

//     out->type = data[0];
//     out->length = (uint16_t)((data[1] << 8) | data[2]);

//     // BUG INJECTED: Read past data buffer if length lies!
//     // Removed: if (size - 3 < out->length) return -1;

//     out->value = &data[3];

//     // Force read value bytes to trigger ASan crash
//     volatile uint8_t dummy = 0;
//     for (size_t i = 0; i < out->length; i++) {
//         dummy = out->value[i]; 
//     }

//     return 0;
// }