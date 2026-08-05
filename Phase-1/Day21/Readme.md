# Day 21 — Small String Optimization (SSO) `LANG`

## What
Build string type storing short strings inline (stack/struct), falling back to heap only past threshold. C way: hand-rolled `sso_string` — tagged union (`bool __is_long` + union of heap-struct / stack-buffer), manual malloc/realloc/free tracking via wrapper functions (`tracked_malloc`, `tracked_realloc`, `tracked_free`) to prove zero-alloc claim, not just assume it. C++ way: `SSOString` class, same union layout, full RAII — copy ctor (deep copy), move ctor (steal ptr, neutralize source), copy/move assignment, destructor, `operator+=`.

## Files
- `sso_string.c` — tagged-union string, 14-byte inline cap (`SSO_STACK_BUFFER_CAP`). Core ops: `sso_init`, `sso_append`, `sso_copy`, `sso_get_cstr`, `sso_free`. Alloc tracked via `malloc_count`/`realloc_count`/`free_count` globals, reset per test.
- `sso_string.cpp` — same design as C++ class, 15-byte inline cap (`STACK_CAP`). Static `alloc`/`dealloc` scoped to class (not global `operator new` override — considered, rejected, see Notes) for same allocation-count verification. Copy ctor preserves source capacity headroom (see Notes on why).
- `Logs.txt` — day log.

## Build

C:
```
gcc sso_string.c -o sso_string
./sso_string
valgrind --leak-check=full ./sso_string
```

C++:
```
g++ -std=c++20 sso_string.cpp -o sso_string_cpp
./sso_string_cpp
valgrind --leak-check=full ./sso_string_cpp
```

## Design

**Tagged union, not inheritance/`std::variant`.** Bool tag (`__is_long` / `is_long`) plus raw union of heap-struct (`ptr`, `size`, `capacity`) and stack-struct (`data[N+1]`, `size`). Matches how real `libc++`/`libstdc++` `basic_string` work — single struct, one branch decide storage mode, no vtable/indirection cost.

**Alloc-count proof, not alloc-count claim.** Every malloc/realloc/free routed through tracked wrappers, counters reset per test, asserted exact after each op (`assert(malloc_count == 0)` on short-string paths, `assert(malloc_count == 1)` on exact transition point). Without this, "SSO works" is just a belief — a `strcmp` passing tells you the *string* is right, not that the *optimization* fired. Caught real regressions this way (below).

**Growth strategy — amortized, not naive-double-target.** First pass grew heap capacity to `newLen * 2` (target-relative). Fixed to `capacity * 2` (current-capacity-relative), clamped up to `newLen` if still short — matches real amortized-growth reasoning (repeated small appends shouldn't force a realloc every time).

**C++ copy ctor preserves source capacity, doesn't tight-copy to size.** First version set `capacity = size` on copy — cheap on memory, but meant *any* post-copy mutation forced an immediate realloc, defeating the point of copying a string with headroom. Switched to copying `other.storage.heap.capacity` verbatim. Tradeoff either way (memory vs. realloc-avoidance); this version chose realloc-avoidance, documented here so it's a decision, not an accident.

**C++ allocation tracking scoped to class, not global `operator new`/`delete` override.** First draft overrode global `operator new`/`operator delete` — technically worked, but counted *every* heap allocation program-wide (any `std::string`, `std::cout` internals, exceptions), not just `SSOString`'s. Fragile — one unrelated allocation anywhere in a test block silently corrupts the count with no warning. Replaced with class-static `alloc`/`dealloc`, called explicitly wherever `SSOString` touches heap. Scoped, matches C wrapper pattern exactly.

**Move ctor/assignment neutralize source explicitly.** Stealing `heap.ptr` isn't enough — source object's destructor still runs at scope exit and must not double-free. After steal, source `is_long` set false, source stack fields zeroed to a safe-empty state. Tested directly: moved-from object checked `.empty()` after move, not just moved-to object checked for correct data.

## Results
Both `sso_string.c` / `sso_string.cpp` — 9 tests C++, 7 tests C, all pass, valgrind clean both:

```
sizeof(SSOString): 32 bytes

[PASS] Stack init / Stack Init & RAII
[PASS] Stack append / Stack Append
[PASS] Stack to Heap transition
[PASS] Direct Heap init & realloc  (C only)
[PASS] Move Semantics              (C++ only)
[PASS] Stack copy & mutate isolation
[PASS] Heap deep copy, pointer isolation & (double-free safety / capacity headroom preservation)
[PASS] Double heap (init / assignment) leak prevention
[PASS] Boundary Exact Limit (14/15 stack -> 15/16 heap)   (C++)
[PASS] Self-move guard verification                        (C++ only)

HEAP SUMMARY: all allocs freed, no leaks possible, 0 errors
```

## Notes / what broke
`sso_copy` (C) first version memcpy'd from **wrong union member** — copied from `src->storage.stack_buffer.__data` when source was in heap mode, instead of `src->storage.heap.__ptr`. Both fields alias the same union memory, so it compiled and even partially ran, but pulled garbage/misinterpreted bytes. No test exercised `sso_copy` in heap mode at the time — bug sat invisible until copy-specific heap-mode test added deliberately (pointer-inequality + mutate-isolation check).

`sso_init` (C) re-init-on-heap-mode leak — calling `sso_init` twice on the same already-heap-allocated struct did `memset` **before** checking/freeing the old heap pointer, wiping the only reference to it. Order flipped: free-if-long, then memset. Proven fixed via explicit malloc/free-count assertions across two consecutive `sso_init` calls on one struct, not just re-running and eyeballing output.

C++ heap-copy capacity mismatch — after switching copy ctor to preserve source capacity, an earlier test asserting a fixed `free_count` after copy+mutate broke, because tight-copy's guaranteed-realloc-on-mutate assumption no longer held. Not a bug in the new code — a stale assumption baked into the test. Rewrote test to force capacity expansion on the source *before* copying, so the "no realloc needed after copy" claim is actually exercised instead of asserted blind.

`void *sso_free(sso_string*)` (C) declared a return type, never returned a value — silent UB under strict compilers. Changed to `void`.

Fresh (garbage) stack-allocated `sso_string` passed straight into `sso_init` without zero-init is still technically UB on the very first call (`__is_long` unread garbage briefly touched before overwrite) — mitigated by contract, not code: caller must `sso_string ss = {0};` before first use. Documented, not fully closed — can't defend against reading garbage stack memory from inside the function itself.

## Todo next
- Compile both under `-fsanitize=address,undefined` — valgrind run done, sanitizer pass still outstanding, called out repeatedly this cycle, not yet done.
- `sizeof(sso_string)` (C side) never measured — only C++ (`32 bytes`) confirmed. Add.
- No exact-boundary test on C side (13/14/15 length) — added for C++, still missing in C.
- Consider `std::string_view`-style non-owning accessor, and `operator==` for real usability past demo stage.