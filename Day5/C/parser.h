#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t type;
    uint16_t length;
    const uint8_t *value;
} Packet;

/* Parse raw bytes into Packet. Return 0 on success, -1 on corrupt input. */
int parse_tlv(const uint8_t *data, size_t size, Packet *out);

#endif