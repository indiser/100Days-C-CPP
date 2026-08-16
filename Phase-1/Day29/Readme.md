# Day 29 — Benchmark Harness `TRADE`

## What
Build a latency micro-benchmark framework in both C and C++ that produces
trustworthy timing numbers, not just a single average. Point of the exercise:
prove the harness itself is honest — warm-up discarded, correct clock,
mean/min/max/stddev/p50/p99 reported, dead-code elimination blocked — before
trusting it to compare anything (needed for Day 24's hashmap-vs-`unordered_map`
claim and the Day 30 checkpoint).

## Files
- `benchmark.c` / `benchmark.h` — C harness. `BenchFunc` function-pointer + `void*`
  ctx interface. `clock_gettime(CLOCK_MONOTONIC_RAW)` per-iteration timing,
  `qsort`-based percentile calc, hand-rolled variance/stddev.
- `benchmark.hpp` — C++ harness. Templated `run_benchmark(Callable&&, warmup, runs)`,
  no `void*`, no function pointer — closures capture their own state.
  `std::vector<double>` + `std::sort` instead of malloc'd array.
- `main.c` / `main.cpp` — 3 real workloads benchmarked per language, plus a
  small-run correctness check and (C++ only) a `std::function` type-erasure
  overhead comparison.
- `Makefile` — build rules, C and C++ targets.
- `Logs.txt` — day log.

## Build

C:
```
gcc -Wall -Wextra -std=c11 -O2 benchmark.c main.c -lm -o bench_c
```

C++:
```
g++ -Wall -Wextra -std=c++20 -O2 main.cpp -o bench_cpp
```

Run: `./bench_c` / `./bench_cpp`

> Note: neither run was pinned to a CPU core or governor — no `taskset -c` /
> `performance` governor applied. Numbers below carry normal turbo-boost and
> scheduler noise. Treat mean/stddev as indicative, not lab-grade.

## Design

| Concern | C | C++ |
|---|---|---|
| Clock | `clock_gettime(CLOCK_MONOTONIC_RAW)` | `std::chrono::steady_clock` |
| Callable interface | `void (*)(void*)` + `void* ctx` | `template<typename Callable>`, raw deduction |
| Sample storage | `malloc`'d `double*`, manually `free`d | `std::vector<double>`, RAII |
| Sort for percentiles | `qsort` + comparator | `std::sort` |
| Anti-optimization | `__asm__ volatile("" : : "g"(var) : "memory")` | same, via `asm volatile` |
| Stats reported | mean, min, max, stddev, p50, p99 | same |

## Workloads tested (both languages)

1. **Cheap op** — LCG state update (`state = state * A + C`), dominated by the
   call/loop overhead itself.
2. **Medium op** — `malloc(256)` + `free()` round-trip cost.
3. **Real op** — Day 24 open-addressing hashmap lookup, keys rotated across
   500 inserted entries (not a single fixed lucky key) so probe-chain length
   varies run to run.
4. **C++ only** — same LCG lambda wrapped in `std::function<void()>` to
   measure type-erasure overhead against the raw template call.
5. **Verification** — small run count (n=100) with `assert(p50 <= p99 <= max)`
   to catch percentile-index math errors that wouldn't show up at n=1,000,000.


## Notes / what broke

**Indirect call overhead is baked into every C number.** `func(ctx)` through a
function pointer costs a few ns per call — negligible against malloc/free or
a hashmap probe chain, but comparable in magnitude to the LCG op itself. The
C "cheap op" number is partly measuring call overhead, not pure LCG cost.
C++'s raw template avoids this — `fn()` on a concrete lambda type can be
inlined directly, no indirection.

**First hashmap benchmark design was rigged and had to be rewritten.** Original
version measured one fixed key with a short probe chain — near-best-case,
told you nothing about real lookup cost. Fixed by rotating through all 500
inserted keys so the benchmark samples a realistic spread of probe-chain
lengths instead of one lucky case.

**`std::function` copies the closure, doesn't reference it.** Assigning a
lambda to `std::function<void()>` copy-constructs the closure object into
type-erased storage. Worked correctly here because the LCG lambda captures
`lcg_state` by reference — but a by-value capture would silently benchmark a
stale copy. Know the mechanic, don't just trust the assignment.

**No CPU pinning applied.** Neither binary was run under `taskset` or a fixed
governor. Turbo boost and OS scheduler noise are present in every number
above — acceptable for this exercise, would not be acceptable input to a
Day 30 checkpoint comparison claim without pinning.

**p99 index math verified, not just asserted to work.** Small-run test
(n=100) checks `p50 <= p99 <= max` explicitly via `assert` in both languages,
catching the case where `(size_t)(runs * 0.99)` could round into `max`'s slot
at low run counts and silently make p99 meaningless.
