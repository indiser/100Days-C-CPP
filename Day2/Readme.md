# Day 2 — Modern Generics

## What
`_Generic` (C11) for manual overload dispatch, `assert` vs `static_assert`, C++20 `concepts`/`requires` to constrain templates.

## Files
- `C/gen.c` — `_Generic`-based `addition()` macro, dispatches int/float to right fn
- `CPP/gen.cpp` — assert vs static_assert demo
- `CPP/gen1.cpp` — `Numeric`/`Printable` concepts, `requires` clauses, concept-constrained fns

## Build

C:
```
gcc gen.c -o gen.exe
./gen.exe
```

C++:
```
g++ -std=c++20 gen.cpp -o gen_assert.exe
g++ -std=c++20 gen1.cpp -o gen1.exe
```

## Notes / what broke

**C — `_Generic`:** no overloading in C, compiler picks fn by arg type at compile time via `_Generic` selector — basically manual name-mangling done by hand instead of compiler. Macro `addition(a,b)` expands to `_Generic((a), int: additioni, float: additionf)(a,b)`. Type not in the list → compile error, not runtime UB. Good.

**C++ — assert vs static_assert:** assert = runtime check, aborts if false. static_assert = compile-time check, fails build. Bug caught in own code: `gen.cpp` does `#define NDEBUG` before `#include <cassert>` — NDEBUG strips assert to no-op. So `assert(sizeof(int) >= 5)` (false on most platforms, int is 4 bytes) never fires, just silently skipped. "Assertion works" prints regardless. Lesson: NDEBUG kills assert entirely, don't define it if you want the check live. static_assert unaffected by NDEBUG — compile-time, always checked. Real difference nailed: static_assert can't be disabled by macro, assert can.

**C++ — concepts/requires:** template with no constraint accepts anything, garbage type errors buried deep in instantiation. `Numeric` concept requires `+`,`-`,`*` return same type — `multiply_add` rejects `std::string` at the call site, not inside the function body. `requires requires{...}` (ad-hoc constraint) checked `c.size()` + `value_type` on `Container` without naming a concept. `Numeric auto val` = abbreviated constrained-auto syntax, same as constrained template param.

## Takeaway
C fakes generics via macro + `_Generic` switch — dispatch resolved at compile time but caller still hand-picks type match, no real constraint system, just a jump table. C++ concepts are actual compiler-enforced contracts — bad type fails at call site with a real diagnostic, not 40 lines of template vomit.

## Todo next
- Rerun gen.cpp assert demo *without* `NDEBUG` — confirm it actually aborts on false condition.
- Day 3: CMake multi-target build (static+shared+exe), C++20 modules experiment.