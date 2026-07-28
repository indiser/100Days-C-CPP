# Day 13 — Binary Serialization (Phase 1) `TRADE`

## What
Struct packing, endianness, manual byte buffers, C++ templates. Built FIX-like market-data serializer in C (fixed-size buffer), and a template-based serialization lib in C++ (generic `write_be<T>`/`read_be<T>`, `std::vector<uint8_t>` wire buffer).

## Files
- `ser_des.c` — `Tick` struct, manual `write_u32_be`/`write_u64_be`/`read_u32_be`/`read_u64_be`, `serialize_tick`/`deserialize_tick` into fixed `uint8_t[32]` buffer
- `ser_des.cpp` — same `Tick`, kept `write_be32`/`write_be64`/`read_be32`/`read_be64` as base primitives, added templated `write_be<T>`/`read_be<T>` dispatch (`if constexpr` on `sizeof(T)`), `TickSerializer` class serializing to/from `std::vector<uint8_t>`
- `endian.c` — separate exploration: `htons`/`htonl`/`ntohs`/`ntohl` (winsock2) vs manual bit-shift equivalents, host-to-network and back
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -Wall -Wextra -O2 -fsanitize=address,undefined ser_des.c -o ser_des
```

C++:
```
g++ -std=c++17 -Wall -Wextra -O2 -fsanitize=address,undefined ser_des.cpp -o ser_des_cpp
```

endian.c (Windows, needs winsock2):
```
gcc -O2 endian.c -o endian -lws2_32
```

## Design

**C version.** Fixed 17-byte wire format (`4 + 8 + 4 + 1` — `TICK_WIRE_SIZE` constant, not magic numbers). Big-endian encode/decode written by hand per width, no shared logic between u32 and u64 paths — every shift typed out twice. `deserialize_tick` bounds-checks against `buf_len` before touching memory.

**C++ version.** Same low-level `write_be32`/`write_be64`/`read_be32`/`read_be64` kept untouched, but wrapped by templated `write_be<T>`/`read_be<T>` that dispatch on `sizeof(T)` at compile time (`if constexpr`, `static_assert` blocks unsupported widths). `TickSerializer::serialize` returns `std::vector<uint8_t>` sized exactly to `TICK_WIRE_SIZE` — no caller-supplied buffer, no buffer-size guessing. Round-trip verified with `assert` on every field, not printf-eyeballing.

**endian.c.** Confirms `htons`/`htonl` match manual bit-shift reversal, both directions. Practical proof that big-endian wire format (what `ser_des` uses) is the same "network byte order" convention as the sockets API.

## Known limitation
- **Vector allocates per call.** `TickSerializer::serialize` heap-allocates a fresh `std::vector` every tick — fine for a lesson, wrong for a real low-latency feed. Real system would serialize into a reused/pre-sized buffer (arena or ring buffer, callback to Day 6/Day 22) to keep this on the zero-alloc hot path.
- **No `weak_ptr` follow-up landed** — carried over from Day 12, still not built. Noting again since it hasn't moved.
- **Endianness is hardcoded big-endian** — no runtime host-endianness detection, no fallback if a future struct field needs native order. Fine for a fixed wire spec, but not generalized.
- **No fuzzing/property test on the parser.** Day 5 taught fuzzing basics; this serializer never got one. A malformed/truncated buffer is only checked for *short* length, not for nonsensical field values (e.g. garbage `side` byte outside {0,1}).
- **`ser_des.c` printf portability** — original had `%llu` cast issue, fixed to `PRIu64`/`%llu` cast consistently; confirm build is warning-clean with `-Wall -Wextra`.

## Notes / what broke
- Endianness mismatch between Linux (`<netinet/in.h>`) and Windows (`<winsock2.h>`) tripped up the `endian.c` exploration first — real, not cosmetic, since the whole point of network byte order is cross-platform agreement, and the *API to get there* isn't even portable by default.
- Logs are thin again — "basically conversion today." Second day in a row logged as easy. Worth being honest with yourself: getting the happy-path round-trip working isn't the same as understanding wire-format design (versioning, variable-length fields, malformed input) — none of that was touched.

## Todo next
- Reuse-buffer variant of `TickSerializer` (no per-call heap alloc) — direct tie-in to Day 6 arena allocator
- Fuzz harness on `deserialize_tick`/`TickSerializer::deserialize` — tie-in to Day 5 fuzzing skill, don't let it stay skipped
- Day 14 — Endianness/binary formats, `GFX`: BMP parser, byte-exact