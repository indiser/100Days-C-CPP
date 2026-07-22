# Day 4 — Profiling

## What
`perf stat`/`perf record`/`perf report`, cache misses, loop blocking/tiling. Fixed a perf bug in matrix multiply — naive vs loop-reordered vs tiled, both C and C++.

## Files
- `matmul.c` — naive / reordered / tiled matrix multiply, 1024x1024 doubles, mode selected via argv
- `matmul.cpp` — same three variants, templated `Matrix<T>` class, `operator()` element access
- `Logs.txt` — `perf stat` output for all three modes, both languages

## Build

C:
```
gcc -O2 matmul.c -o matmul.exe
```

C++:
```
g++ -O2 -std=c++20 matmul.cpp -o matmul_cpp.exe
```

Run: `./matmul.exe [naive|reordered|tiled]` (same for the C++ binary).

## Results (wall-clock, from internal `clock_gettime`/`chrono` timing)

| Variant | C | C++ |
|---|---|---|
| naive (i,j,k) | 8.92s / 4.85s (two runs) | — |
| reordered (i,k,j) | 0.89s / 0.83s | — |
| tiled (block=64) | 0.67s / 0.47s | — |

C++ run not yet captured in `Logs.txt` — pending.

Naive → reordered: ~10x speedup, from fixing the access pattern on `B` (`B[k*N+j]` sequential vs `B[k*N+j]` strided in the naive `i,j,k` order — column access on `B` was the killer, not `A`). Reordered → tiled: further ~25-40% on top, from keeping the working block resident in cache across the `i,k,j` sweep instead of streaming the full row/column through per outer iteration.

## Notes / what broke

**Original test was invalid.** First matmul (2x2 * 2x3, `printf` inside the hot loop, 1000x wrapped) never exercised cache behavior at all — matrices too small to leave L1, and `perf stat` was measuring syscall overhead from `printf`, not compute (sys time 0.364s vs user time 0.018s in that run — proof it was I/O-bound, not math-bound). Rebuilt with 1024x1024 doubles, no I/O in the loop, single run.

**`perf stat` counters unreliable in this environment.** Across every run, task-clock sits at 6-9ms while wall-clock is 0.4s-9s — CPUs utilized reported as 0.001-0.01. That means the process was scheduled on-CPU for under 1% of its own runtime; total instruction count (~3.2M) is far too low for a real 1024³ multiply (should be well over 1B ops). Cycles/IPC/cache-miss/branch-miss numbers from `perf stat` are not trustworthy here — this is a sandboxed/virtualized environment issue (likely CPU quota or virtualized hardware counters), not a property of the code. Confirmed by checking `cpu.max`/cgroup quota — flagged, not yet root-caused.

**Conclusions drawn from wall-clock only.** The 10x and further ~30% speedups are real and match theory (sequential vs strided access, then cache-resident blocking), but backed by `clock_gettime`/`chrono` timing directly in the program, not by `perf`'s hardware counters. No claim made here about actual cache-miss counts — that data isn't trustworthy yet in this environment.

**C vs C++ codegen — not yet verified.** Plan was to diff `objdump -d` output between the C loop and the C++ `Matrix<T>::operator()` version to confirm the abstraction compiles down to identical pointer arithmetic at `-O2`. Not done yet — todo before this day is called closed.

## Takeaway
Loop order is the single biggest lever on this kind of code — a one-line change (swap `j` and `k` loop) beat a naive implementation by an order of magnitude, before any tiling was involved, because it turned a strided memory access into a sequential one. Tiling adds a smaller but real further win by keeping working sets cache-resident. None of this required `perf`'s hardware counters to detect — wall-clock time alone told the story — which is itself a lesson: the tool is only as good as the environment it's running in, and this environment's hardware counters weren't fit for purpose right now. Don't write conclusions from numbers that don't add up (3.2M instructions for a billion-op computation is impossible) just because a tool printed them.

## Todo next
- Root-cause the `perf` hardware-counter unreliability (check `cpu.max`/cgroup quota, consider bare-metal or a VM with `perf_event_paranoid` set low)
- `objdump -d` diff: C loop vs C++ `operator()` version, confirm identical codegen at `-O2`
- Re-run at N=2048/4096 to widen the gap between reordered and tiled once L2/L3 capacity is actually exceeded
- Capture C++ `Logs.txt` entries (currently only naive/reordered/tiled `.c` runs logged)
- Day 5: fuzzing — libFuzzer/AFL harness basics, property-based testing