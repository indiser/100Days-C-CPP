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
gcc -O3 StringRoutine.c -o s3.exe
```

C++ (must use g++, not gcc — linker needs libstdc++ for chrono/iostream):
```
g++ -O0 StringRoutine.cpp -o cs0.exe
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

## Asm diff
```
objdump -d -M intel s0.exe > s0.asm
objdump -d -M intel s3.exe > s3.asm
diff s0.asm s3.asm
```
Note: redirect `>` on Windows writes UTF-16 — convert before diffing on Linux/WSL:
```
iconv -f UTF-16LE -t UTF-8 s0.asm > s0_utf8.asm
```
Ignore diff noise in CRT startup (`__mingw_*`, `_amsg_exit`, etc) — only `<reverse>:` block matters.

## Result
- -O0: naive stack-based loop, redundant index recompute every iter, stack spill for temp.
- -O3: strength-reduced to two-pointer walk (front/back), fully register-resident, ~half instruction count.
- Wall-clock confirms: -O3 measurably faster than -O0.

## Todo next
- Same asm-diff exercise on C++ `reverse` vs `reverseT<char>` — confirm near-identical codegen at -O3 (zero-cost abstraction claim).
- Move heavy dev to WSL — native Windows breaks on `perf`, `mmap`, `epoll`, etc from Phase 2 on.