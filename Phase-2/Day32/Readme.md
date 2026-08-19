# Day 32 — Atomics & Ordering `TRADE`

## What
Build an atomic price ticker shared between a writer thread (simulated market feed) and a reader thread (polling consumer), using explicit memory ordering instead of a mutex. C way: `stdatomic.h`, `atomic_store_explicit`/`atomic_load_explicit`, manual release/acquire pairing on a `seq` field to publish `price`+`volume`. C++ way: same struct as `std::atomic<long>` members, same ordering pattern via `.store()`/`.load()`. Both add a CAS retry loop (`update_if_higher`) and a micro-benchmark comparing `seq_cst` vs `release`/`acquire` vs `relaxed` throughput.

## Files
- `AtomicPriceTracker.c` — cache-line-padded `Ticker` struct (`price`, `volume`, `seq`, each isolated to its own 64-byte line), writer publishes payload with `memory_order_relaxed` then flips `seq` with `memory_order_release`, reader spins on `stop_flag`/`seq` with `memory_order_acquire`; separate `update_if_higher` CAS loop; `bench_ordering` measures ops/sec per ordering.
- `AtomicPriceTracker.cpp` — same struct and pattern as `alignas(64)` C++ struct, `std::atomic<long>` members, same CAS loop as a free function taking `std::atomic<long>&`, same benchmark harness using `std::chrono::steady_clock`.
- `atomics.cpp` — scratch file, basic `fetch_add`/`compare_exchange_weak` sanity checks before touching the real ticker. Not part of the main deliverable.
- `Logs.txt` — day log.
- `Results.txt` — two separate runs' output, C and C++ (see Results below).

## Build

C:
```
gcc AtomicPriceTracker.c -o tracker_c -lpthread
./tracker_c
```

C++:
```
g++ -std=c++20 AtomicPriceTracker.cpp -o tracker_cpp -lpthread
./tracker_cpp
```

Scratch file (optional, not part of main deliverable):
```
g++ atomics.cpp -o atomics_scratch -lpthread
./atomics_scratch
```

## Design

**`seq` is the release/acquire hinge, not `price` or `volume` directly.** Payload fields get written with `memory_order_relaxed` — cheap, no ordering guarantee on their own. The `seq` store afterward uses `memory_order_release`, which stops the compiler/CPU from reordering the payload writes to after it. Reader loads `seq` with `memory_order_acquire`, and only then is it safe to read `price`/`volume` — acquire stops those reads from moving before the `seq` check. This is the one edge that actually matters; everything else rides on it.

**Two-field publish is not one atomic operation.** `price` and `volume` are separate `_Atomic`/`std::atomic` variables — there's no atomic "update both at once." The `seq` flip is what makes the *pair* consistent from the reader's point of view: reader only trusts `price`/`volume` after observing a new `seq`, so it never catches one field mid-update relative to the other, even though nothing prevents that at the field level in isolation.

**CAS retry loop reused as the "update if better" primitive.** `update_if_higher` loads current, compares, and calls `compare_exchange_weak` in a loop rather than a single check-then-set — a plain `if (candidate > current) price = candidate` has a race window between the check and the write. `compare_exchange_weak` refreshes `current` on failure automatically, so the loop just retries with the up-to-date value until it wins or the candidate stops being higher. This exact shape comes back for day 33 (lock-free queue) and day 39 (spinlock).

**Padding to cache-line size is intentional, not decoration.** `price`, `volume`, `seq` each get padded out to 64 bytes so writer and reader threads touching different fields aren't fighting over the same cache line (false sharing, day 7 callback). Without the padding the benchmark numbers would be measuring cache contention as much as ordering cost.

## Results
C run (`Results.txt`, first block):
```
reader: saw 547441 distinct updates, torn=0
...
seq_cst:        77929719.08 ops/sec
release/acquire: 78919497.38 ops/sec
relaxed:        79365425.80 ops/sec
```

C++ run (`Results.txt`, second block):
```
reader: saw 809728 distinct updates
...
seq_cst:         57896118.73 ops/sec
release/acquire: 60670223.96 ops/sec
relaxed:         55159479.85 ops/sec
```

CAS demo identical both languages, as expected — same six candidates, same accept/reject pattern, `best` lands on 500 in both.

## Notes / what broke
Ordering benchmark numbers don't cleanly separate — `seq_cst` sometimes beats `release`/`acquire` here (C++ run: seq_cst 57.9M vs relaxed 55.2M, relaxed *lower*). Single-core uncontended loop on one thread doesn't stress the thing memory ordering actually costs — that cost shows up under multi-core contention (writer and reader on separate cores fighting over the same cache line), not a solo loop incrementing a local counter. Numbers here are noise-level, not a real signal — logged honestly rather than reading a story into them that isn't there.

Reader's distinct-update count swings hard run to run (547441 vs 809728, earlier single-shot version saw as low as 2–3) — spin-poll reader with no backoff means the count is just "how many times did the reader's while-loop happen to land between writer stores," pure scheduling luck, not a correctness signal. `torn=0` in the C version is also dead weight — `torn_detected` is declared, never incremented, never actually checked for a torn read. Logged as a gap, not fixed yet: the field gives false confidence sitting in the output looking like a real check.

`atomics.cpp` is scratch — large blocks of commented-out earlier attempts (`comp_exchange`, two-thread `count()`) left in place across the file instead of deleted once superseded. Same discipline lapse flagged on day 31. Cleaned enough to compile and run but not cleaned enough to be case-study code.

## Todo next
- Benchmark needs to actually contend — two threads hammering the same atomic from separate cores, not one thread looping alone, before the `seq_cst` vs `relaxed` cost claim means anything.
- Wire up `torn_detected` for real, or drop the field — currently pure decoration.
- Reader should log every distinct `seq` it sees (or at minimum a min/max gap) instead of just a running count, so a dropped/skipped tick is visible instead of averaged away.
- Day 33 (lock-free queue) is where this CAS loop shape gets stress-tested against actual concurrent producers, not a single writer.