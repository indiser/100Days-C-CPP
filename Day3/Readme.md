# Day 3 — Build Systems

## What
CMake multi-target build: `add_library`, `add_executable`, `target_link_libraries`, static vs shared libs, `target_include_directories`.

## Files
- `C/CMakeLists.txt` — builds `math_functions_static` (STATIC), `math_functions_shared` (SHARED), `app_static`, `app_shared`
- `C/src/math_functions.c`, `C/include/math_functions.h` — add/sub/mul/div/remainder
- `C/src/main.c` — driver calling all five fns
- `CPP/CMakeLists.txt` — same structure, C++ sources
- `CPP/src/math_functions.cpp`, `CPP/include/math_functions.hpp`
- `CPP/src/main.cpp` — driver using `iostream`

## Build

C:
```
cd C
mkdir build && cd build
cmake ..
cmake --build .
```

C++: same, in `CPP/` dir.

## Notes / what broke

**Version mismatch:** `cmake_minimum_required(VERSION 4.1)` — no such CMake version exists. Copy-pasted config without checking `cmake --version` first. Fixed to actual installed version.

**Static vs shared, proven via `ldd`:**
```
app_shared.exe → depends on libmath_functions_shared.dll
app_static.exe → no math_functions dep, code baked in
```
Static lib = object code copied into exe at link time, self-contained, larger binary, no runtime dep. Shared lib = code lives in separate `.dll`, loaded at runtime, exe stays small but breaks if `.dll` missing from search path.

**Verified by deleting the dll:** removing `libmath_functions_shared.dll` next to `app_shared.exe` and running it — fails to launch, missing dependency. `app_static.exe` unaffected, runs standalone. Confirms shared-lib deploy risk directly instead of trusting docs.

**Windows extra artifacts:** MinGW/CMake on Windows also emits `.dll.a` (import library) alongside `.dll` — needed at link time to resolve symbols even though actual code loads from `.dll` at runtime. Not present on Linux shared builds (`.so` only).

**`target_include_directories(... PUBLIC include)`:** `PUBLIC` means anything linking this lib also inherits the include path — so `app_static`/`app_shared` see `math_functions.h` without restating the include dir themselves.

## Takeaway
Static linking trades binary size for deploy simplicity — one file, no missing-dep failure mode. Shared linking trades that safety for smaller exe + shared code across processes, but ships as a set, not a file — miss the `.dll` and it's a runtime crash, not a compile error. CMake itself is just a config generator, not scary — confusion was from not reading own generated file, not from the tool.

## Todo next
- C++20 modules experiment (skipped this round — static/shared fundamentals first)
- Day 4: profiling with `perf`, cache-miss fix in matrix-multiply