# Day 26 — Zero-Copy Parsing `TRADE`

## What
Build a zero-copy market-data feed parser — walk a raw byte stream and extract ticks without a single heap allocation. No `malloc`, no `new`, no `std::string` construction anywhere in the hot path. Fields get pointer+length views (or `string_view` slices) straight into the caller's buffer, not copies. C way: raw `const uint8_t*` pointer arithmetic, manual `memcpy`-based unaligned reads, byteswap macros wrapping `__builtin_bswap32/64`. C++ way: same layout via `std::string_view` slicing, no functional change in allocation behavior — the point of the exercise is that `string_view` buys safety/ergonomics, not zero-copy itself, since the C version was already zero-copy.

## Files
- `zeroCopyParse.c` — `MarketTickView` (raw pointer+len for symbol), `read_be32`/`read_be64` via `memcpy`+bswap, `parse_feed` loops buffer via callback (`on_tick` fn pointer), returns consumed byte count so caller can stash unconsumed tail bytes for next packet.
- `zeroCopyParse.cpp` — `FeedParser` class, same loop logic, `MarketTickView.symbol` as `std::string_view` instead of pointer+len pair, callback via `std::function`.
- `Logs.txt` — day log.

## Build

C:
```
gcc -Wall -Wextra -fsanitize=address,undefined -g zeroCopyParse.c -o c_parse_test && ./c_parse_test
```

C++:
```
g++ -std=c++17 -Wall -Wextra -fsanitize=address,undefined -g zeroCopyParse.cpp -o cpp_parse_test && ./cpp_parse_test
```

## Design

**Byteswap intrinsics are compiler-specific, not a detail to skip.** First draft used `_byteswap_ulong`/`_byteswap_uint64` — MSVC-only, doesn't compile on gcc/clang. Fixed with `#if defined(_MSC_VER)` branching to `__builtin_bswap32`/`__builtin_bswap64` on the Linux toolchain actually used here.

**Unaligned reads via `memcpy`, not a raw pointer cast.** First draft did `*(uint32_t*)(raw_buf + 4)` — casts a `uint8_t*` to `uint32_t*` and dereferences, which is a strict-aliasing violation and can fault on unaligned access depending on platform/codegen. Fixed by copying the 4/8 raw bytes into a local scalar via `memcpy` before byteswapping. This `memcpy` is not a violation of "zero-copy" — it's copying a handful of bytes into a register-sized local, not duplicating the payload; the symbol field still points straight into the original buffer.

**Feed parser, not a single-struct-cast demo.** First draft only parsed one hardcoded 16-byte tick, called once. Spec asked for a feed parser — real feeds are a stream of many ticks back to back. Fixed with `parse_feed` looping `offset` through the whole buffer while a full `TICK_SIZE` (16 bytes) remains, calling `on_tick` per record.

**Partial trailing tick handled by returning consumed byte count, not by erroring out.** Real feed reads land mid-tick at packet boundaries constantly — `offset + TICK_SIZE <= len` stops the loop before reading past a partial record, and `parse_feed` returns `offset` so the caller knows exactly how many bytes were consumed and can carry the leftover tail into the next `recv()` — matches how real feed handlers reassemble TCP/UDP fragments instead of pretending every read lands on a tick boundary.

**Callback pattern instead of returning a `vector`/array of ticks.** Returning a collected list would mean allocating storage for the results — defeats the entire point of the exercise. `on_tick(&tick)` fires per-record instead, so tick storage lifetime is the caller's problem, not this function's.

**`string_view` in C++ doesn't buy less code, it buys a bundled pointer+length with a safer interface.** `MarketTickView.symbol` went from `const char* symbol; size_t sym_len;` (two fields, C) to `std::string_view symbol` (one field, C++) — same zero-copy guarantee, no allocation difference, but the view can't be accidentally split from its length or misused as a C-string across a boundary that isn't null-terminated (the payload here has none).

## Results

Both languages build clean under ASan+UBSan, zero heap allocations in the parse path (verified by code inspection — no `malloc`/`new`/`std::string` construction anywhere in `parse_feed`/`read_be32`/`read_be64` or their C++ equivalents):

```
--- Processing Feed Stream ---
Tick -> Symbol: AAPL, Price: 100, Qty: 1000
Tick -> Symbol: MSFT, Price: 400, Qty: 500

Stream size: 37 bytes
Consumed   : 32 bytes
Unconsumed : 5 bytes (save to ring buffer for next packet)
```

```
--- Processing C++ Feed Stream ---
Tick -> Symbol: AAPL | Price: 100 | Qty: 1000
Tick -> Symbol: MSFT | Price: 400 | Qty: 500

Stream size: 37 bytes
Consumed   : 32 bytes
Unconsumed : 5 bytes
```

## Notes / what broke

First draft used MSVC byteswap intrinsics on a Linux gcc toolchain — didn't compile at all. Caught before it ran, not by a test.

First draft cast raw buffer bytes directly to `uint32_t*`/`uint64_t*` and dereferenced — compiles, "works" on x86 by luck, but is UB (strict aliasing + potential unaligned fault). Fixed with `memcpy`-into-local before every multi-byte read, both languages.

First draft parsed exactly one hardcoded tick and called it a "feed parser" — no loop, no multi-tick handling, no partial-tick logic. Spec explicitly says feed parser; a single struct-cast on a fixed 16-byte array doesn't earn that name. Rebuilt around `parse_feed` looping the whole buffer.

`tick.symbol` (C) / `MarketTickView.symbol` (C++) is a pointer/view into the original buffer, valid only for the duration of the callback. Nothing currently stops a caller from stashing that pointer/view past the callback and reading it after the buffer's been overwritten by the next `recv()` — silent stale-data bug, not a crash, so nothing would flag it. No test exercises this failure mode yet.

## Todo next
- No test proves the "stash pointer past callback, buffer gets overwritten, read is garbage" failure mode actually happens — currently just reasoned about, not demonstrated.
- No fuzz/property test feeding random buffer lengths (0, 1, 15, 16, 17, huge) through `parse_feed` to confirm the boundary math never over-reads — `offset + TICK_SIZE <= len` looks right but hasn't been adversarially tested.
- C and C++ versions never diffed for byte-for-byte identical output on the same input beyond eyeballing the printed lines — should assert programmatically.