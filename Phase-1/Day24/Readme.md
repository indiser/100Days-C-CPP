# Day 24 — High-Perf Hash Maps `TRADE`

## What
Build open-addressed hash maps from scratch — no chaining, no per-node heap alloc — the layout real low-latency symbol lookup tables use. Flat contiguous slot array, linear probing, tombstone-based delete, resize-on-load-factor. C way: fixed global table, `Slot` array with `EMPTY`/`OCCUPIED`/`DELETED` state, manual probe loop. C++ way: `SymbolTable<K, V>` — templated, FNV-1a over key bytes, same tombstone/resize logic, benchmarked head-to-head against `std::unordered_map`.

## Files
- `LookUpTable.c` — flat `Slot` array (no pointer indirection), power-of-2 capacity, `&mask` instead of `%`, 3-state slots, `symbol_table_delete`, auto-resize at 0.7 load.
- `LookUpTable.cpp` — templated `SymbolTable<K,V>`, same probing/tombstone/resize design, `std::optional<V>` return on lookup, includes benchmark harness vs `unordered_map`.
- `Logs.txt` — day log.

## Build

C:
```
gcc -Wall -Wextra -fsanitize=address,undefined -o lut LookUpTable.c && ./lut
```

C++:
```
g++ -std=c++17 -O2 -Wall -Wextra -o lutcpp LookUpTable.cpp && ./lutcpp
```

## Design

**Flat array of values, not array of pointers — this is the whole point of the day.** First draft (`SymbolTable *table[MAX_POOL_SIZE]`) stored a pointer per slot, `malloc`'d individually on insert. That's chasing a heap pointer on every lookup — same cache-miss cost as separate-chaining, defeats the entire premise of "cache-aware open addressing." Fixed to `Slot table[CAPACITY]` — contiguous, one allocation, probe sequence walks memory linearly.

**3-state slots (`EMPTY`/`OCCUPIED`/`DELETED`) instead of 2-state, because delete without tombstones breaks probing correctness.** A naive open-addressed delete that just marks a slot "empty" truncates the probe chain — any key that collided past that slot and got pushed further down the sequence becomes unreachable, since lookup stops at the first `EMPTY` it sees. Tombstone (`DELETED`) keeps the probe chain intact: lookup skips over it, insert is free to reclaim it.

**Power-of-2 capacity + bitmask instead of modulo.** `index & (capacity - 1)` is one instruction; `% capacity` for non-power-of-2 sizes is a division. First draft used `MAX_POOL_SIZE = 10` with `%` — correct but slower for no reason. Switched capacity to powers of 2 starting at 16.

**Resize triggers on `count + tombstones`, not just `count`.** Tombstones still occupy probe-chain space and degrade worst-case lookup even though they don't count as "used" slots. Load factor check includes them so a delete-heavy workload doesn't quietly turn every lookup into a near-full linear scan while `count` looks low.

**Insert reuses the first tombstone seen during probing, not the eventual empty slot.** If probe hits `DELETED` at index 3 then `EMPTY` at index 7, key gets written to slot 3 — shortens future probe chains for this key's bucket instead of letting tombstones accumulate at the front of every chain.

**Global mutable table in C (`g_table`) instead of pointer-passing everywhere.** Matches the existing `symbol_table_create/insert/lookup/delete/destroy` API shape from the pre-fix draft — kept the interface, fixed the internals, no API churn for the sake of it.

**C++ version templated over `K, V`, not just `<string, double>` hardcoded.** Costs one constraint: `hashKey` iterates `key`'s bytes, so `K` must be byte-iterable (works for `std::string`, wouldn't work for `int` un-adapted). Documented via the FNV-1a-over-bytes implementation rather than a `static_assert` — acceptable gap for a day-24 exercise, would need a proper hash trait/specialization system before this template saw production key types beyond strings.

## Results

Smoke test, both languages — insert, update, miss, delete, forced resize (16→128 capacity via 50 extra inserts), post-resize lookup:
```
AAPL price: 185.00 (updated correctly)
NVDA price: 721.33
MSFT not found
NVDA deleted, not found
capacity after growth: 128, count: 52
AAPL still found after resize: 185.00
```
ASan/UBSan clean in both C and C++ builds.

Benchmark, N=100,000 symbols, 5-rep mean, custom table vs `std::unordered_map`:
```
insert  custom: 31395.4 us   unordered_map: 37337.0 us
lookup  custom:  5664.9 us   unordered_map: 15790.5 us
```
Custom table wins both, lookup by ~3x — flat-array cache locality beating chained-bucket-plus-heap-node lookup, as expected going in.

## Notes / what broke
First C draft crashed in `symbol_table_destroy` — dereferenced `table[i]->occupied` without checking `table[i] != NULL` first. Any unfilled slot in a sparse table segfaults on teardown. Fixed to null-check before touch.

First C draft's core data structure (`SymbolTable *table[N]`, pointer array + per-slot `malloc`) was a correctness-only implementation that missed the actual assignment — "cache-aware" was the stated goal and the design wasn't. Caught on review, not by a test — nothing in the smoke test would have flagged it, since pointer-chasing and flat-array both return correct answers. Worth remembering: passing tests isn't the same as satisfying the spec.

First C draft had no delete function and no tombstone state at all — `DELETED` didn't exist. Added after the fact, which meant retrofitting the probe-chain-truncation fix rather than designing around it from the start. Should design the state enum before writing the first probe loop next time a delete requirement is known up front.

First C++ draft wouldn't compile — `std::shuffle` used without `<algorithm>` included (transitively available in some stdlib versions via `<random>`, not guaranteed). Caught at build time, not runtime — cheap catch, but a reminder to include exactly what's used rather than relying on transitive headers.

Benchmark harness reports mean only, discards the 5 samples per `timeIt` call after averaging — no stddev, no variance visibility, no run-to-run stability signal. Also single load pattern only (sequential `SYM0`..`SYM99999` keys, no adversarial-collision test, no sweep across load factors 0.5/0.75/0.9 like the topic list called for). Numbers above are real and directionally correct, but this isn't yet the rigorous benchmark day 29's harness is supposed to produce — flagged, not fixed, since day 29 is where that tooling gets built properly.

## Todo next
- Consider robin-hood probing as a second variant to compare against plain linear probing — spec called out "probing strategies" plural, only one was built.