# Day 1 — Optimization / Codegen

## What
Benchmark string-reverse routine across gcc/g++ opt levels (`-O0`–`-O3`), read asm diff, compare C vs C++ (plain vs templated) codegen.

## Files
- `C/StringRoutine.c` — plain reverse, timed with `clock()`
- `CPP/StringRoutine.cpp` — plain + templated reverse, timed with `std::chrono::steady_clock`

## Build

C:
```
gcc -O0 StringRoutine.c -o s0.exe
gcc -O1 StringRoutine.c -o s1.exe
gcc -O2 StringRoutine.c -o s2.exe
gcc -O3 StringRoutine.c -o s3.exe
```

C++ (must use g++, not gcc — linker needs libstdc++ for chrono/iostream):
```
g++ -O0 StringRoutine.cpp -o cs0.exe
g++ -O1 StringRoutine.cpp -o cs1.exe
g++ -O2 StringRoutine.cpp -o cs2.exe
g++ -O3 StringRoutine.cpp -o cs3.exe
```

## Run / time (PowerShell — no `time` cmd on Windows)
```
Measure-Command { ./s0.exe }
Measure-Command { ./s3.exe }
```

## Compile stages (inspect each by hand)
```
gcc -E StringRoutine.c -o StringRoutine.i   # preprocessed (headers expanded)
gcc -S StringRoutine.c -o StringRoutine.s   # asm, human-readable
gcc -c StringRoutine.c -o StringRoutine.o   # object file, unlinked
gcc StringRoutine.o -o StringRoutine        # link -> final exe
```
(C++ side: swap `gcc` -> `g++`, same 4 commands.)

## Asm diff
```
objdump -d -M intel s0.exe > s0.asm
objdump -d -M intel s3.exe > s3.asm
```
Do NOT use `fc /n /c /w` on Windows — the `/c` flag makes it diff char-by-char, unreadable garbage. Use real line diff instead: convert UTF-16 -> UTF-8 (Windows `>` redirect writes UTF-16), then diff on Linux/WSL:
```
iconv -f UTF-16LE -t UTF-8 s0.asm > s0_utf8.asm
iconv -f UTF-16LE -t UTF-8 s3.asm > s3_utf8.asm
diff s0_utf8.asm s3_utf8.asm
```
Ignore diff noise in CRT startup (`__mingw_*`, `_amsg_exit`, etc) — only `<reverse>:` block matters.

## Results

### C — timing (StringRoutine.c)
| Flag | Time |
|---|---|
| -O0 | 2104.00 ms |
| -O1 | 630.00 ms |
| -O2 | 625.00 ms (production standard, most optimized) |
| -O3 | 624.00 ms |

Big cliff O0->O1 (~3.3x), then flattens — O2/O3 basically same for this routine, nothing left for -O3's extra vectorization/inlining aggression to grab here.

### C++ — timing (StringRoutine.cpp, non-template vs template)
| Flag | Non-template | Template |
|---|---|---|
| -O0 | 3352.25 ms | 3677.81 ms |
| -O1 | 690.57 ms | 780.25 ms |
| -O2 | 660.99 ms | 653.05 ms (production standard, most optimized) |
| -O3 | 669.59 ms | 660.68 ms |

At -O0 template is *slower* than plain — no inlining yet, template machinery costs something raw. By -O2/-O3 gap closes, template even edges ahead slightly (noise-level) — confirms zero-cost abstraction claim only holds once optimizer's actually on.

C++ -O0 (3352ms) noticeably worse than C -O0 (2104ms) — unoptimized C++ carries more overhead (iostream, extra abstraction layers) than raw C. Gap nearly disappears by -O2/-O3 — optimizer strips it back down.

### Asm — reverse() O0 vs O3
- -O0: naive stack-based loop, reloads index from stack every iter, recomputes `n-1-i` twice per iter, stack spill for temp `t`.
- -O3: collapsed to two-pointer walk (front ptr up, back ptr down), fully register-resident, no stack access in loop body, ~half instruction count.

## Todo next
- Real (line-based) asm diff on C++ `reverse` vs `reverseT<char>` — confirm near-identical codegen at -O2/-O3, matches timing data above.
- Move heavy dev to WSL — native Windows breaks on `perf`, `mmap`, `epoll`, etc from Phase 2 on.