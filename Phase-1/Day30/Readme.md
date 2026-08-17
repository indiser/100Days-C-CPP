# Day 30 — Checkpoint: LRU Price Cache `TRADE`

## What
Combine Day 6–29 work into one checkpoint: hashmap + doubly-linked-list,
O(1) get/put, eviction policy. C version hand-rolls the hash table
(chaining, no `unordered_map`). C++ version templates the same design over
`K`/`V`, uses `unordered_map<K, Node*>` for lookup, sentinel-node DLL for
O(1) splice. Both benchmarked against Day 29's harness pattern.

## Files
- `lru.c` — hash table (mod `TABLE_SIZE`, chaining) + DLL, capacity-bound
  eviction of true LRU (list tail). Verbose prints, small demo run.
- `lru.cpp` — templated `LRUCache<K,V>`, sentinel head/tail nodes,
  `unordered_map` for O(1) lookup, RAII destructor frees all nodes.
- `bench_c.c` / `bench_cpp.cpp` — standalone benchmark drivers, no printf in
  hot path, `TABLE_SIZE` bumped to 16384 for the C table to keep load factor
  sane at `CAP=10000`.
- `Results.txt` — raw output, C and C++ runs.

## Build

C:
```
gcc -Wall -Wextra -O2 bench_c.c -o bench_c
```

C++:
```
g++ -Wall -Wextra -std=c++20 -O2 bench_cpp.cpp -o bench_cpp
```

Run: `./bench_c` / `./bench_cpp`

> Note: no `-O2` confirmed applied consistently across both prior runs, no
> CPU pinning (`taskset`), no fixed governor. Numbers below carry normal
> scheduler/turbo noise — same caveat as Day 29, not lab-grade.

## Design

| Concern | C | C++ |
|---|---|---|
| Lookup structure | hand-rolled hash table, chaining, `TABLE_SIZE=16384` | `std::unordered_map<K,Node*>` |
| Eviction structure | intrusive DLL, raw `prev`/`next` pointers | intrusive DLL, sentinel `head`/`tail` nodes |
| Node lifetime | `malloc`/`free`, manual, freed on evict + `freeCache` | `new`/`delete`, freed on evict + destructor |
| Key type | fixed `int` | templated `K` |
| Move-to-front | unlink + relink, no realloc | same |

## Benchmark results

| | Throughput | Mean latency |
|---|---|---|
| C | 52.32 M ops/sec | 19.11 ns/op |
| C++ | 4.80 M ops/sec | 208.37 ns/op |

**~10.9x gap, C over C++.** Bigger than expected — worth being suspicious of
before writing it down as "C is just faster":
- `unordered_map` is node-based: every `put()` on a new key does a heap
  alloc for the map's internal node *plus* the `Node` for the DLL — two
  allocs where C's chained table does one (`malloc(sizeof(Node))`, table
  slot is just a pointer write, no separate alloc).
- Every `unordered_map` op re-hashes through a virtual-ish bucket/hash
  interface with more indirection than my flat chained array.
- Default `unordered_map` load factor / rehash growth wasn't tuned or
  `reserve()`'d up front — first 8000 warm-up inserts likely triggered
  multiple rehashes (each one rehashing every existing entry). C table was
  sized once (`TABLE_SIZE=16384`) and never resizes.

**Not yet ruled out — don't trust the 10x number until checked:**
- Whether `bench_cpp.cpp` was actually compiled `-O2` (not confirmed same
  flags as `bench_c.c` in the run that produced `Results.txt`).
- `mp.reserve(CAP)` never called — rerun with it, isolates rehash cost from
  genuine per-op overhead.
- No CPU pinning — a 10x claim on noisy scheduler data is not solid enough
  to write into a portfolio README as fact.

## Notes / what broke

**First C version indexed a global array by raw key value — not a hash
table.** `hashMap[key]` with `key` used directly as array index worked only
because demo keys were tiny ints. Real checkpoint requires a real hash
function + collision handling; rewrote with `hash_func()` + chaining before
this was acceptable.

**First C++ version leaked on every eviction and every update.** `delNode()`
only unlinked, never `delete`d — cache that evicts by leaking memory is a
broken eviction policy, not a working one. Fixed: `delete victim` on evict
path, destructor walks the full list freeing every node including sentinels.

**`limit == 0` edge case in C++ constructor.** Unguarded, first `put()` on a
zero-capacity cache would evict `tail->prev` (the `head` sentinel itself) —
corrupts the list before it's ever used. Guarded with `if (limit == 0) limit
= 1`, but that's a patch, not a real answer — a zero-capacity cache
probably shouldn't silently become capacity-1. Flag for later, not solved.

**Ran under `-fsanitize=address,undefined` — clean on both.** Confirms no
leaks, no UB, no double-free, no use-after-free on evict path. This was
verified, not assumed.

**Benchmark honesty gap vs Day 29 harness.** Day 29 built a proper harness
(warmup discard, mean/min/max/stddev/p50/p99, dead-code-elimination guard).
This checkpoint's `bench_c.c`/`bench_cpp.cpp` report mean latency only, no
percentiles, no variance, no anti-DCE guard on the compiler. That's a
regression against the tool built two days ago — reuse the Day 29 harness
here instead of a second, weaker one-off timer next time.