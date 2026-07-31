# Day 16 — String Interning & Copy-on-Write Strings `DS`

## What
Two O(1)-comparison string techniques: hash-based interning pool in C (`string_interning.c`, `pool.h`), and reference-counted copy-on-write string in C++ (`cow_string.cpp`, `cow_string.hpp`). Bonus: hash function bakeoff (`StringHashs.c`) comparing djb2, FNV-1a, and MurMur3.

## Files
- `pool.h` — `StringPool` struct (open-addressed bucket array), interface for `pool_create`, `pool_intern`, `pool_destroy`
- `string_interning.c` — open-addressing hash table with linear probing, FNV-1a hash, dedupes strings so equal content shares one pointer; `pool_intern` returns `NULL` if pool is full instead of resizing
- `StringHashs.c` — three hash implementations (djb2, FNV-1a, MurMur3 32-bit) run against the same input string for comparison
- `cow_string.cpp` / `cow_string.hpp` — `CowString` class, ref-counted `StringData` buffer, copy/assign just bump `ref_count`, `detach_if_shared()` clones only on write via non-const `operator[]`
- `Logs.txt` — day notes

## Design
**Interning:** equal strings collapse to one heap allocation, so subsequent equality checks are pointer comparison (O(1)) instead of `strcmp` (O(n)). Collisions resolved by linear probing; lookup and insert share one loop that walks buckets until it hits `NULL` (free slot) or a `strcmp` match.

**COW:** const `operator[]` reads the shared buffer directly, no detach. Non-const `operator[]` calls `detach_if_shared()` first — if `ref_count > 1`, allocates a private copy before returning a mutable reference, so writes on one instance never leak into another. Copy constructor and copy-assignment are both O(1): pointer copy + refcount increment, no allocation.

## What broke / what's still open
Checked the files, flagging honest:

- `pool_intern` returns `NULL` silently when `pool->count >= pool->capacity` — no resize, no error signal beyond `NULL`. Caller has no way to tell "pool full" apart from "malloc failed inside `strdup`". Same failure mode, indistinguishable.
- Linear probing with no tombstone/deletion path — `pool.h` exposes no `pool_remove`, consistent with current use, but worth stating: this pool only grows, never shrinks, until `pool_destroy`.
- Load factor unbounded — nothing stops `pool->count` from approaching `pool->capacity`, at which point linear probing degrades toward O(n) per insert. No resize-on-load-factor logic.
- FNV-1a is defined identically in both `string_interning.c` and `StringHashs.c` — duplicated, not shared via header. Drifts if one copy gets fixed and the other doesn't.
- `cow_string.cpp`: `detach_if_shared()` decrements `buffer_->ref_count` on the *old* shared block after allocating the new one, but never checks whether that decrement dropped the shared block to `0` refs — if this is ever called somewhere the old block could legitimately hit zero here, it leaks. Currently unreachable given call sites, but the guard other destructors use (`if (ref_count == 0) delete`) is missing from this one path.
- `CowString` has no move constructor / move assignment — every move degrades to a copy (ref bump), missing a free optimization C++11 rvalue refs would give.
- Log notes claim COW "banned since newer version of cpp" — true for `std::string`'s COW (killed by C++11's small-string-optimization + thread-safety requirements), but worth being precise: it's `libstdc++`'s pre-C++11 COW implementation that got deprecated/removed, not "COW as a technique," which this file correctly still demonstrates as valid for custom types.

## Todo next
- Add resize-on-load-factor (e.g. rehash at 0.7) to `StringPool`
- Fix missing zero-refcount check in `detach_if_shared()`
- Hoist FNV-1a into a shared header, drop the duplicate
- Move semantics for `CowString`