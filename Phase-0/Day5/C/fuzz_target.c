#include <stdint.h>
#include <stddef.h>
#include "parser.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    Packet pkt;
    parse_tlv(Data, Size, &pkt);
    return 0;
}