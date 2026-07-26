# Day 10 — Cache-Friendly Layout (Phase 1) `GFX`

## What
AoS (array-of-structs) vs SoA (structure-of-arrays) data layout — cache locality, memory access patterns, benchmarking methodology. C and C++ versions, particle system update loop as the test case.

## Files
- `aos.c` / `soa.c` — C benchmarks, `struct particle { float x,y,z; uint64_t color; }` array vs four separate arrays
- `aos.cpp` / `soa.cpp` — same logic, C++ using `std::vector<Particle>` vs four `std::vector`s
- `Logs.txt` — raw notes + perf runs from the day

## Build

C:
```
gcc -O2 aos.c -o aos
gcc -O2 soa.c -o soa
```

C++:
```
g++ -O2 aos.cpp -o aos_cpp
g++ -O2 soa.cpp -o soa_cpp
```

Timing via `clock_gettime(CLOCK_MONOTONIC)` (C) / `std::chrono::steady_clock` (C++). Cache behavior via `perf stat -e cache-misses,cache-references`.

## Design

**AoS.** One array of `Particle` structs, 20 bytes each (`x,y,z` floats + `color` uint64). Update loop touches only `x,y,z`, drags `color` along every cache-line load anyway — that's the theoretical AoS penalty.

**SoA.** Four separate arrays, one per field. Update loop touches only the `x,y,z` arrays, never loads `color` — should mean fewer wasted bytes per cache line, fewer cache-misses, faster loop.

**Accumulator pattern.** Loop sums `x+y+z` into a running total instead of just overwriting local vars — stops the compiler from dead-code-eliminating the whole loop at `-O2`. Original first draft of `aos.c` didn't do this, loop would've vanished under optimization. Fixed before first real run.

**N = 1<<20** — bumped from an initial 1024, big enough to blow past L1/L2 and force real memory traffic instead of everything living in cache the whole time.

## Results (WSL, not native Linux — see below)

| | AoS (C) | SoA (C) | AoS (C++) | SoA (C++) |
|---|---|---|---|---|
| ns/particle | 4.58–7.08 | 2.66–2.87 | 4.47 | 3.83 |
| cache-misses | 130k–152k | 139k–162k | 138525 | 162054 |
| miss rate | — | — | 38.98% | 39.73% |

Timing consistently favors SoA — 1.2x to 2.5x faster across runs, both languages. That part lines up with theory.

Cache-miss counts do **not** line up with theory. SoA shows equal or *higher* raw misses and miss rate than AoS in every run, despite touching less data per element logically. If cache locality were the real story here, SoA should show fewer misses, not more.

## Notes / what broke — and what's still unresolved

**Measured on WSL, not native Linux.** This is the actual problem, not a footnote. `perf`'s own "time elapsed" reported 0.2–1.2 seconds while the program's own internal timer reported 3–7 milliseconds for the same run — a 50–300x mismatch between perf's wall clock and the program's own clock. That gap alone means the perf counters here can't be trusted as ground truth. Runs also weren't consistent with each other run-to-run (AoS/SoA gap shrank from 2.5x to 1.16x between two otherwise-identical test sessions), which is what noisy measurement looks like, not what a stable hardware effect looks like.

**Cache-miss data contradicts the hypothesis.** Wrote "SoA has fewer cache misses" in an earlier log entry before actually checking the numbers — that was wrong, and it's fixed here. The honest read: timing improved with SoA, but the cache-miss counters do not explain why, at least not on this measurement setup. Something else — possibly auto-vectorization differences, possibly allocator/page-layout effects from `malloc`-ing 4 separate arrays vs 1 struct array, possibly prefetcher behavior — is driving the time gap, and it wasn't isolated.

**Only one run per config, mostly.** Cache counters are noisy by nature. Single-sample numbers already produced two contradictory-looking runs (7.1 vs 4.47 ns/particle for the same AoS binary conceptually, across sessions) — not enough evidence either way without repeated runs and a median.

**Struct too small to prove much.** `Particle` is only 20 bytes, `color` adds 8 of it. That's not a large waste ratio — a real-world case with more unused fields per struct would show a bigger, more reliable gap between layouts.

## Known limitation
Not verified on native Linux. WSL's perf layer produces internally inconsistent numbers (wall-clock mismatch, contradictory cache-miss direction) and shouldn't be treated as a real measurement of cache behavior — only the relative timing trend is worth anything here, and even that needs more runs to trust.

## Todo next
- Rerun full benchmark set on native Linux (dual-boot / VM / cloud box), 10 runs each, median not single-sample
- Add `cache-references` alongside `cache-misses` for miss rate, not just raw count (done for C++, still owed for C)
- Widen `Particle` struct (extra unused fields) to increase AoS waste ratio and produce a clearer signal
- Compare `-O0` vs `-O2` to see how much auto-vectorization is masking the true memory-bound effect
- Day 11 — Lock-free basics, `TRADE` domain: lock-free order queue, CAS/`compare_exchange`, ABA problem.