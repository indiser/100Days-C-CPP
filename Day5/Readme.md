# Day 5 — Fuzzing (Phase 0 close)

## What
libFuzzer harness on hand-rolled TLV parser, C and C++ versions. Goal: find bounds bug via coverage-guided fuzzing, then compare raw-pointer C vs `std::span` C++ for same parse logic.

## Files
- `parser.h` / `parser.c` — C TLV parser, `parse_tlv(data, size, Packet*)`, manual bounds check
- `parser.cpp` — C++ version, `parse_tlv_cpp(std::span<const uint8_t>)`, returns `std::optional<PacketCPP>`
- `fuzz_target.c` — libFuzzer entry, calls `parse_tlv`
- `fuzz_target.cpp` — libFuzzer entry, calls `parse_tlv_cpp`, asserts span-size and pointer-bounds invariants on success
- `Logs.txt` — both fuzzer runs, C then C++

## Build
Needed WSL Linux — MSYS2 Clang missing libFuzzer runtime (`-fsanitize=fuzzer` not supported there).

C:
```
clang -g -fsanitize=fuzzer,address,undefined fuzz_target.c parser.c -o fuzz_c
./fuzz_c
```

C++:
```
clang++ -std=c++20 -g -fsanitize=fuzzer,address,undefined fuzz_target.cpp -o fuzz_cpp
./fuzz_cpp
```
(`fuzz_target.cpp` `#include`s `parser.cpp` directly instead of linking — single TU, no header for the C++ parser.)

## Results

| | C | C++ |
|---|---|---|
| max cov hit | 21 | 92 |
| max ft hit | 22 | 110 |
| exec/s (steady) | ~650-700k | ~120k (climbing, still ramping at last pulse) |
| execs run | 33M+ | 16M+ |
| crash found | none | none after startup |

C run: coverage flatlined at cov 21 by run #365, stayed flat through 33M+ execs. Bounds check (`size - 3 < out->length`) holding — no ASan trip.

C++ run: one stack frame through `std::span::operator[]` logged right at init (`#2 INITED`, before `NEW_FUNC` entries) — not a confirmed crash, just first coverage trace through that function; run continued past it to 16M+ execs with no fuzzer-reported crash/abort after. Higher coverage (92 vs 21) for same input space — span/optional/subspan machinery pulls in more branches to cover, not necessarily more real logic paths, so cov numbers aren't directly comparable across the two binaries.

## Notes / what broke

**Bug-trap version never fuzzed.** `parser.c` has a second implementation commented out at the bottom — bounds check deliberately removed, would read past buffer if `length` lies. Never uncommented and run; the guarded version was fuzzed instead. So this campaign confirms the *current* bounds check holds, not that the fuzzer can catch the injected bug — that's still an open loop.

**exec/s C vs C++ not apples-to-apples.** C hit 700k exec/s, C++ topped out lower — but C++ log shows exec/s still rising every pulse (5k → 11k → 21k → 41k → 72k → 119k), never plateaued like C did. Comparing final numbers understates C++ throughput; would need to run both to the same wall-clock/exec count and let both plateau before calling a winner.

**No crash = inconclusive, not proof of correctness.** 33M execs with 3-byte-minimum, mostly-tiny inputs (corpus stayed at 3/7-9 bytes) is a shallow input space for a TLV format — length field goes up to 65535 but corpus never grew past single digits. Coverage plateau this early on this few corpus entries suggests the fuzzer exhausted the easy paths, not the input space. Should confirm with `-max_len` raised and/or a seed corpus with larger declared lengths.

## Takeaway
Manual bounds check in C and `subspan`-based check in C++ both held under fuzzing — same logic, same guard (`size - 3 < length`), just enforced by hand vs by the type. Neither found the bug, because neither ran the version with the bug. The real test (bug-trap build) is still undone — that's the actual point of Day 5 and it's not closed yet.

## Phase 0 status: not done
Fuzzing infra works (harness, build, ASan/UBSan wired up) but the one thing it was built to prove — catch the injected overflow — hasn't been run. Closing this day as "phase 0 complete" without that would be closing on infra working, not on the bug being caught.

## Todo next
- Uncomment bug-trap `parse_tlv` in `parser.c`, rebuild `fuzz_c`, confirm ASan actually catches the read past buffer
- Raise `-max_len` (e.g. 65536) and seed corpus with large declared `length` values to reach the interesting part of the input space
- Let C++ run to plateau (currently still climbing) before comparing exec/s against C
- Resolve the `operator[]` stack frame at init — confirm it's a coverage trace, not a swallowed early crash
- Phase 1: move off toy TLV parser to a real format (or start on the next language/tool in the syllabus)