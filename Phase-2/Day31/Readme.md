# Day 31 — Thread Lifecycle `TRADE`

## What
Spawn, run, and correctly tear down a fixed-size worker pool consuming a shared queue of simulated market ticks. C way: raw `pthread_create`/`pthread_join`, manual mutex around a shared index, explicit error propagation via `pthread_exit` return values. Also side-quest: minimal two-thread `pthread_create`/`pthread_join`/`pthread_detach` sanity check before building the real pool. C++ way: same worker-pool design wrapped in a `WorkerPool` class, `std::thread` + `std::mutex`, RAII join-on-destruction, non-copyable/non-movable to keep thread ownership unambiguous.

## Files
- `MarketPool.c` — 4-worker pool pulling ticks off a shared array under `queue_mutex`; checks return values on every pthread call (`lock`/`unlock`/`create`/`join`/`destroy`); partial-spawn failure only joins threads actually created (`spawned` counter); worker-side errors propagate out via `pthread_exit((void*)1)` and are checked in `main` via `retval`.
- `threads.c` — scratch file, two-thread sanity check (`myTurn`/`yourTurn`) proving basic create/join/detach mechanics before touching the real pool.
- `MarketPool.cpp` — same pool as a `WorkerPool` class; `std::mutex queue_mutex` guards the shared index, second `print_mutex` guards `std::cout`; destructor joins all workers (`join_all`, `noexcept`); copy and move both explicitly deleted.
- `Logs.txt` — day log.

## Build

C:
```
gcc MarketPool.c -o pool_c -lpthread
./pool_c
```

Scratch file (optional, not part of main deliverable):
```
gcc threads.c -o threads -lpthread
./threads
```

C++:
```
g++ -std=c++20 MarketPool.cpp -o pool_cpp -lpthread
./pool_cpp
```

## Design

**Pull-pattern over pre-assignment.** Workers loop and grab the next tick off a shared index under lock, rather than being handed a fixed slice up front. Self-balances if workers finish at different rates — no worker sits idle holding unclaimed work while another still has a backlog.

**Partial-spawn failure must only clean up what actually exists.** First-pass instinct is to just `return 1` the moment `pthread_create` fails. Wrong — any threads already spawned before the failure are now orphaned, nobody joins or detaches them. Real fix: track a `spawned` counter, only join threads actually created, break out of the create-loop on first failure instead of trying to recover mid-loop.

**Worker-side errors need an explicit path out.** A failed `pthread_mutex_lock` inside a worker doesn't crash anything by default — it just silently corrupts control flow if ignored. Fixed by having the worker call `pthread_exit((void*)1)` and having `main` check `retval` on every join. Note: while one worker dies mid-run, the other three keep going and finish their share — throughput silently degrades with no indication to `main` until the final join. Acceptable at this scope, but the seed of why concurrent error handling gets hard: errors surface far from where they happened.

**`std::cout` chained `<<` is not atomic across threads — proved, not assumed.** First C++ pass streamed `"Worker " << id << ...` directly to `cout` inside the print section. Output came back visibly interleaved mid-line — digits from one tick's price landing inside another tick's line. Not a data race on the tick queue (every tick still processed exactly once, no dupes/skips) — the race is on the sequence of separate `operator<<` calls against one shared stream. Fixed by building the full line into an `ostringstream` first, then doing exactly one `std::cout <<` under a dedicated `print_mutex`, kept separate from `queue_mutex` so print contention doesn't serialize queue access.

**RAII join in the destructor, copy/move deleted on purpose.** `~WorkerPool()` calls `join_all()` unconditionally (`noexcept`) so no code path can leak a running thread. Copy is deleted because duplicating a `WorkerPool` would duplicate ownership of live `std::thread` objects, which is nonsensical. Move is also deleted for now — reassigning thread ownership mid-run adds complexity this day isn't testing; revisit if a later day needs a movable pool.

## Results
`MarketPool.c` / `MarketPool.cpp` run (both):
```
Worker 0 processed tick 1: AAPL @ $150.00
Worker 2 processed tick 4: AAPL @ $153.00
Worker 3 processed tick 3: AAPL @ $152.00
Worker 1 processed tick 2: AAPL @ $151.00
...
All ticks processed cleanly. Exit.
```
Both versions process all 12 ticks exactly once, output order genuinely interleaved (confirms real concurrent execution, not faked/serial), clean exit code 0 on success path.

## Notes / what broke
`std::cout` interleaving bug — chained `<<` calls on a shared stream from multiple threads produced garbled, spliced-together output lines (not a queue race — every tick still counted exactly once). Root cause: streaming is a sequence of separate calls, not one atomic write. Caught immediately because it's visually obvious; the same class of bug against a non-visual shared resource (log file, metrics counter) would corrupt silently instead.

Partial pthread_create failure — original C version returned immediately on a mid-loop `pthread_create` failure without joining threads already spawned, leaking them. Fixed with a `spawned` counter so cleanup only touches threads that actually exist.

Dead commented-out code left sitting in `MarketPool.c` above the real implementation across two revisions — not a technical bug, a discipline lapse. Stale comments rot and risk being trusted or accidentally reintroduced. Removed.

## Todo next
- Second mutex (`print_mutex`) works but fully serializes output — fine at 4 workers/12 ticks, revisit if a later day needs high-throughput logging without stalling workers on I/O (day 53, async logging, is the real fix for this pattern).
- No thread-pool reuse yet — pool is spawn-once/join-once for a fixed tick count. Day 34 (real thread pools) is where persistent pools with task queues get built properly.
- `WorkerPool` move-disabled for now — revisit if a later design needs to hand pool ownership across scopes.