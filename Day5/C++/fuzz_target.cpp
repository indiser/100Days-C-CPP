#include <cstdint>
#include <cstddef>
#include <span>
#include <cassert>
#include "parser.cpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    std::span<const uint8_t> input(Data, Size);
    auto res = parse_tlv_cpp(input);

    if (res.has_value()) {
        // Property invariant 1: Span size MUST equal packet length
        assert(res->value.size() == res->length);
        
        // Property invariant 2: Value pointer within input bounds
        assert(res->value.data() >= Data);
        assert(res->value.data() + res->value.size() <= Data + Size);
    }

    return 0;
}