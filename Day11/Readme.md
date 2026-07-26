# Day 11 — Lock-Free Basics (Phase 1) `TRADE`

## What
Lock-free data structures vs mutex-based ones. CAS (`compare_exchange`), ABA problem + fix, memory ordering (`relaxed`/`acquire`/`release`/`seq_cst`). Built SPSC and MPMC bounded queues, C and C++, stress-tested under real thread contention with ThreadSanitizer.

## Files
- `SPSC.c` / `SPSC.cpp` — single-producer single-consumer ring buffer
- `MPMC.c` / `MPMC.cpp` — multi-producer multi-consumer bounded queue, Dmitry Vyukov's sequence-per-slot design
- `MPMC_test.c` / `MPMC_test.cpp` — stress test: 4 producer + 4 consumer threads, 100k items each, full correctness verification (no drops, no dupes, no corruption)
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -O2 -pthread SPSC.c -o spsc
gcc -O2 -pthread MPMC.c -o mpmc
gcc -Wall -Wextra -O2 -fsanitize=thread -pthread MPMC_test.c -o mpmc_test_c
```

C++:
```
g++ -O2 -pthread SPSC.cpp -o spsc_cpp
g++ -O2 -pthread MPMC.cpp -o mpmc_cpp
g++ -Wall -Wextra -O2 -fsanitize=thread -pthread MPMC_test.cpp -o mpmc_test_cpp
```

## Design

**SPSC.** Ring buffer, single writer/reader, CAS-based head/tail advance mostly as a formality (no real contention with one thread each side). `next_tail == head` → full, `head == tail` → empty.

**MPMC.** Vyukov bounded queue — each slot carries its own `sequence` counter, not just a shared head/tail. Producer CAS-claims a slot on `tail` with `relaxed` (cheap, just reserving an index), writes the payload, *then* publishes with a `release` store to `slot.sequence`. Consumer does the mirror: `acquire`-loads `sequence` before trusting `slot.data`, CAS-claims `head` relaxed, reads data, republishes `sequence + CAPACITY` for the next lap. Sequence diff (`seq - pos`) tells push/pop whether the slot is ready, stale (retry), or the queue is full/empty (bail).

**Why relaxed CAS + release store, not release CAS.** The index CAS just reserves a slot — no data has been written yet, nothing to publish. Publishing is a separate, later step: write data first, then release-store the sequence. Order matters — release before the data write leaves a window where a consumer can see the "ready" signal and read the slot before the producer has actually written it. That's the exact bug this day's C version had for two rounds running.

## Bugs found and fixed (worth keeping — this is the actual lesson)

1. **Publish-before-write race.** First few passes released the slot (via `tail` CAS or `sequence` store) *before* writing `slot->data`/`data[tail]`. Consumer could see the "advanced" index and read stale/uninitialized memory before the producer's write landed. Fix: write data first, publish second, always.
2. **Spin-forever on full/empty.** Early SPSC draft looped forever instead of returning `false` when the ring was full — no consumer running meant permanent busy-wait, not a real "queue full" signal.
3. **No proof under contention.** First few versions ran single-threaded in `main()` — push once, pop once — and called it done. That proves nothing; lock-free bugs only surface under real interleavings.
4. **`SPSC.c` still carries the publish-order bug** (`tail` CAS fires `release` *before* `q->data[tail] = val`) — not yet fixed, flagged here instead of silently shipped. `SPSC.cpp` has it fixed (data written before the `release` store). Todo: port the fix back to the C version.

## Verification

`MPMC_test.c` / `.cpp`: 4 producers × 100,000 items + 4 consumers, each item checked for range validity, zero duplicates (bitmap), and sum-matches-expected. Compiled and run under `-fsanitize=thread -pthread`:

```
gcc -Wall -Wextra -O2 -fsanitize=thread -pthread MPMC_test.c -o mpmc_test_c && ./mpmc_test_c
g++ -Wall -Wextra -O2 -fsanitize=thread -pthread MPMC_test.cpp -o mpmc_test_cpp && ./mpmc_test_cpp
```

Result: both clean under TSan, `SUCCESS: 400000 items verified (0 dropped, 0 duplicated, 0 corrupted)`.

## Notes / what broke
- WSL doesn't support TSan — moved to native Linux for this day's testing (per Day 10's WSL `perf` mismatch, this tracks: WSL keeps being the wrong tool for anything touching low-level correctness/perf instrumentation).
- Multiple compile passes needed to catch the publish-order race — it doesn't show up in single-threaded runs, only under `-fsanitize=thread` with real contention. Confirms the trap from the daily ritual: code that "runs and prints the right thing once" is not evidence of lock-free correctness.
- Spent most of the day here — hardest day so far by a wide margin. No single good end-to-end tutorial found; pieced together from CAS/ABA/memory-order references separately.

## Known limitation
- `SPSC.c` publish-order bug unfixed (see above) — real race, not yet patched.
- `MPMC_test` correctness harness proves *this run* was clean; TSan absence of report is strong but not exhaustive — different thread counts/scheduling could still expose something. Re-run periodically, don't treat one green run as permanent proof.
- Only bounded queues built — no dynamic/growable lock-free structure attempted yet.

## Todo next
- Port publish-before-write fix into `SPSC.c`
- Re-run `MPMC_test` at higher thread counts (8, 16) and multiple repeated runs, not just one session
- Try swapping some `acquire`/`release` for `seq_cst` and back, measure perf delta, to actually feel the cost/benefit tradeoff rather than just getting it "correct by pattern-matching Vyukov's design"
- Day 12 — Smart pointer internals, `SYS`: reference counting, move semantics, RAII, manual refcounted pointer, hand-rolled `unique_ptr`/`shared_ptr`