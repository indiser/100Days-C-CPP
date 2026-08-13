# Day 28 — Undefined Behavior `SEC`

## What
Build catalog of classic UB categories in both C and C++, catch each under UBSan/ASan at `-O0`, then rebuild at `-O3` with sanitizers off to prove the optimizer actually exploits the UB — not just that it's "technically illegal." Point of the exercise: show same source, two build flags, two different observable behaviors. That divergence is the whole lesson, not a side effect.

## Files
- `ub_catalog.c` — 5 UB triggers: signed overflow, strict aliasing (via noinline helper forcing real reorder), shift-out-of-bounds, null deref, unaligned access.
- `ub_cpp_catalog.cpp` — C++-specific divergence: `reinterpret_cast` punning vs `union` punning (still UB in C++, common myth says otherwise) vs defined punning (`memcpy`/`std::bit_cast`), plus pure-virtual-call-in-constructor (no C equivalent — vtable not fully built yet).
- `Makefile` — debug build (`-O0 -fsanitize=undefined,alignment` / `undefined,address` for C++, `-fno-sanitize-recover=all`) and release build (`-O3`, sanitizers off).
- `Logs.txt` — day log.
- `Results.txt` — full run output, debug vs release, C and C++.

## Build

C:
```
gcc -Wall -Wextra -std=c11 -g -O0 -fsanitize=undefined,alignment -fno-sanitize-recover=all ub_catalog.c -o ub_debug
gcc -Wall -Wextra -std=c11 -O3 ub_catalog.c -o ub_release
```

C++:
```
g++ -Wall -Wextra -std=c++20 -g -O0 -fsanitize=undefined,address -fno-sanitize-recover=all ub_cpp_catalog.cpp -o ub_cpp_debug
g++ -Wall -Wextra -std=c++20 -O3 ub_cpp_catalog.cpp -o ub_cpp_release
```

Run each case 1–5 (C) or 1–4 (C++) via `./ub_debug <n>` / `./ub_release <n>`.

## Results

### C — debug (UBSan) vs release (-O3)

| # | Category | UBSan catch | -O3 behavior |
|---|---|---|---|
| 1 | Signed overflow | `signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'` | Silently wraps: `-2147483648` |
| 2 | Strict aliasing | Not flagged by plain UBSan — proven by result divergence instead | `0` at -O0 → `100` at -O3 (optimizer reuses cached load, assumes `*i`/`*f` can't alias) |
| 3 | Shift OOB | `shift exponent 35 is too large for 32-bit type 'unsigned int'` | Silently returns `0` |
| 4 | Null deref | `load of null pointer of type 'int'` | Segfault (no sanitizer net) |
| 5 | Unaligned access | `store to misaligned address ... requires 4 byte alignment` | Works fine, prints `42` — x86 tolerates unaligned access; UB ≠ guaranteed crash, means no guarantee at all. ARM/strict-alignment target would SIGBUS here instead. |

### C++ — debug (UBSan+ASan) vs release (-O3)

| # | Category | Debug | Release |
|---|---|---|---|
| 1 | `reinterpret_cast` aliasing | `0` | `100` — same divergence as C, same root cause |
| 2 | `union` inactive-member read | `0` (both builds) — still UB per C++ standard even though result looks "fine"; C99 explicitly permits this as an extension, C++ never does | `0` |
| 3 | `memcpy` / `std::bit_cast` punning | `0` / `0` — defined behavior, correct at every opt level | `0` / `0` |
| 4 | Pure virtual call in constructor | `pure virtual method called`, aborts | Aborts identically — vtable-during-construction UB not opt-level-dependent, it's a lifetime rule enforced at runtime by libstdc++ regardless of `-O` |

## Notes / what broke

**Strict aliasing needed a rewrite to actually prove anything.** First draft wrote through a `float*` then immediately read through the aliased `int*` in the same few lines — compiler had no reason to cache/reorder, so debug and release matched and nothing was demonstrated. Fixed by pulling the write/read into a `noinline` helper forcing two genuinely separate memory operations the optimizer is free to reorder around — only then did `-O3` diverge from `-O0` (`0` → `100`), proving the optimizer actually assumed no-alias and cached the earlier value.

**Plain UBSan doesn't catch aliasing violations at all.** Neither `-fsanitize=undefined` nor `undefined,alignment` flagged case 2 in either language — no diagnostic printed even though the violation is real. Aliasing has to be caught by behavior-diffing across opt levels (or a dedicated `-fsanitize=strict-aliasing` where available), not by reading sanitizer output. Real finding, not a tooling gap to paper over.

**Union punning result matched at both opt levels — still UB, just didn't visibly break here.** `u.i` after writing `u.f` printed `0` in both debug and release. Standard still calls this UB in C++ (unlike C's explicit allowance) regardless of whether this compiler/arch happened to keep it stable — absence of visible divergence isn't the same as absence of UB. Noted, not treated as "safe because it matched."

**Pure-virtual-ctor-call is a lifetime UB category C literally cannot express** — no vtables, no construction-order concept. Aborts identically at every opt level since libstdc++ enforces it as a runtime check (calls `__cxa_pure_virtual`), not something codegen quality changes. Genuinely different failure mode from every other UB category tested — deterministic-and-loud instead of silently-plausible-wrong-answer.

**Unaligned access "worked" on this arch — don't read that as safe.** x86 has hardware support for unaligned loads/stores at a perf cost, so `-O3` printed the correct value with zero visible symptom. Same code on ARM/strict-alignment target: SIGBUS. This is the clearest case in the catalog that "ran fine" and "defined behavior" are not the same claim.

## Todo next
- Run `-O1`/`-O2` between the two extremes tested — right now it's a binary -O0-vs-O3 jump; worth checking whether any category flips gradually or all-at-once.
- Cross-check case 5 (unaligned access) on an ARM target or QEMU to actually observe the SIGBUS claimed above instead of asserting it from documentation.
- Try `-fsanitize=strict-aliasing` (or equivalent) directly instead of only proving aliasing UB via opt-level result divergence.
- Valgrind pass on both C and C++ binaries — none run yet for Day 28, unlike Day 27's full leak-check coverage.