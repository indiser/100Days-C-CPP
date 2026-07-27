# Day 7 — Alignment & Padding (Phase 1) `TRADE`

## What
Struct alignment, padding, and false sharing. C and C++ versions. Goal: understand how compiler lays out struct members, how to force cache-line alignment on purpose, and prove false sharing cost with a real concurrent benchmark — not just theory.

## Files
- `alignment.c` — cache-aligned `MarketTick` struct via `__attribute__((aligned(64)))`, offsets/addresses printed
- `alignment.cpp` — same via `alignas(64)`, standard-portable version
- `diff.c` — false sharing benchmark: two threads bump adjacent unaligned counters vs two threads bump cache-line-separated counters, timed
- `false_sharing_cpp.exe` — C++ version of same benchmark using `std::thread`
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -O2 alignment.c -o alignment
gcc -O2 -pthread diff.c -o diff
```

C++:
```
g++ -O2 alignment.cpp -o alignment_cpp
g++ -O2 -pthread false_sharing.cpp -o false_sharing_cpp
```

## Design

**Struct layout.** `MarketTick` fields ordered biggest-to-smallest (`uint64_t`, `double`, `uint64_t`, `uint64_t`, `char[8]`) — no internal padding, 40 bytes raw, rounded to 64 by the alignment requirement.

**Forcing alignment.** C used `__attribute__((aligned(64)))` — `alignas(64)` on a bare struct declaration (no variable attached) throws a "useless alignas in empty declaration" warning, since alignas must attach to a member, variable, or the type itself via `struct alignas(N) Name {...}` form. C++ version uses `alignas(64)` directly on the type — standard, portable, no compiler extension needed.

**False sharing proof.** Two structs: `TickBad` (plain `uint64_t`, no padding — adjacent array elements land on the same 64-byte cache line) vs `TickGood` (`alignas(64)`/`aligned(64)` — each element forced onto its own line). Two threads hammer separate counters in each version, timed with `clock_gettime`/`std::chrono::steady_clock`. Bad version pays MESI cache-line invalidation ping-pong on every write even though the threads touch logically unrelated data; good version doesn't.

## Results
```
sizeof(TickBad)  = 8,  alignof = 8
sizeof(TickGood) = 64, alignof = 64
bad_arr  stride: 8 bytes   (same cache line)
good_arr stride: 64 bytes  (separate cache lines)

False-sharing (unaligned) time: <t_bad> sec
Cache-aligned (no false-sharing) time: <t_good> sec
Slowdown factor from false sharing: <X>x
```
(fill in actual numbers from your run)

## Notes / what broke

**`alignas` on empty struct declaration** — first attempt wrote `alignas(64) struct ali {...};` with no variable, got "useless alignas in empty declaration" warning. Fixed by attaching `alignas` to a member, a variable, or using `struct alignas(N) Name {...}` type-level form instead.

**Confused alignment vs size** — assumed `alignas(N)` on a variable would change `sizeof`. It doesn't, unless the requested alignment forces tail padding. Struct already sized as a multiple of the alignment showed no visible change, which looked like `alignas` "did nothing" — it was working, just not visibly on that particular size.

**`%d` for `sizeof`** — `sizeof` returns `size_t`, printed with `%d` originally. Wrong width, UB on 64-bit. Fixed to `%zu`.

**gcc-specific `alignof(tick)` on a variable** — C11 standard only allows `alignof` on a type-name, not an expression. GCC accepts it as an extension. Works here, not portable — `alignof(Tick)` is the standard-legal form.

## Takeaway
Padding isn't just "wasted bytes" trivia — the real cost shows up under concurrency. Two threads touching *different* variables that happen to share a cache line pay real, measurable latency from cache-coherency traffic (false sharing), even with zero actual data contention. `alignas`/`__attribute__((aligned))` is the tool to force layout on purpose instead of leaving it to compiler default — the difference between a struct that "happens to work" and one that's safe under real concurrent load, which is the whole point in a low-latency trading context.

## Phase 1 status: day 7 closed
C and C++ versions built: aligned struct + offset/address dump, plus a concurrent false-sharing benchmark proving the cost in wall-clock time, not just definitions.

## Todo next
- Day 8 — `mmap`, `DB` domain: memory-mapped log grep tool.