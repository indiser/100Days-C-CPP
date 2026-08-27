# Day 39 — Locks From Scratch (Spinlock via `atomic_flag`/CAS, RAII Guard, Low-Latency Book Access)

## What
Spinlock built from raw atomics in C and C++, benchmarked head-to-head against `pthread_mutex`/`std::mutex` on a shared order-book struct under concurrent writer threads. C version: `atomic_flag` + `_mm_pause()` backoff, manual acquire/release calls. C++ version: same primitive wrapped as a class, plus an RAII `LockGuard` (`std::lock_guard` clone) so lock/unlock can't be forgotten on an early return.

## Files
- `SpinLocks.c` — `spinlock_t` (single `atomic_flag` member), `acquire`/`release` using `atomic_flag_test_and_set_explicit`/`atomic_flag_clear_explicit` w/ `memory_order_acquire`/`memory_order_release`, `cpu_relax()` (`_mm_pause()` on x86) spun in the wait loop, 4 `pthread_t` threads hammering a shared `OrderBooks` struct 100,000 iterations each
- `MutexLock.c` — same `OrderBooks` struct, same 4-thread/100k-iteration shape, `pthread_mutex_t` instead of the spinlock — control group
- `SpinLock.cpp` — `Spinlock` class (`std::atomic_flag`, same acquire/release semantics as C), `LockGuard` RAII wrapper (ctor locks, dtor unlocks, copy deleted), 2 `std::thread`s updating a shared `OrderBook` 100,000 iterations each
- `MutexLock.cpp` — same shape, `std::mutex`+`std::lock_guard` instead of custom spinlock — control group

## Build
```
gcc -O2 -pthread SpinLocks.c -o spin_c
gcc -O2 -pthread MutexLock.c -o mutex_c
g++ -O2 -pthread SpinLock.cpp -o spin_cpp
g++ -O2 -pthread MutexLock.cpp -o mutex_cpp
```

## Run
```
./spin_c
./mutex_c
./spin_cpp
./mutex_cpp
```

## Design
- Both spinlocks are test-and-set (TAS), not test-then-test-and-set (TTAS) — every failed attempt retries the atomic RMW directly instead of spinning on a plain load first. Cheaper to write, worse under real contention (more coherency traffic) — noted as the next thing to fix, not shipped as final.
- `cpu_relax()` (`PAUSE` instruction) added in the spin loop — doesn't change correctness, reduces power draw and memory-order mis-speculation penalty on x86 while spinning.
- `memory_order_acquire` on lock, `memory_order_release` on unlock — minimum ordering needed for correctness, avoided `seq_cst` on purpose to keep it honest to what a real low-latency spinlock would use.
- C version: explicit `acquire`/`release` call pairs, no RAII — every exit path from the critical section has to remember to call `release` by hand. C has no destructor to lean on.
- C++ version: `LockGuard` ties unlock to scope exit — copy constructor/assignment deleted so a guard can't be duplicated and double-unlock. Same pattern as Day 38's `unique_lock`, applied to a hand-rolled lock instead of `std::mutex`.
- Shared state in both languages is a 4-field order-book struct (`bid_price`, `price`/`ask_price`, `volume`, `quantity`) — small critical section on purpose, this is the regime spinlocks are supposed to win in.

## Results

**C, 4 threads, 100,000 iters/thread:**
```
MutexLock:  Volume 400000 ✓  Quantity 2000000 ✓  Execution time: 0.02s
SpinLock:   Volume 400000 ✓  Quantity 2000000 ✓  Execution time: 0.03s
```
Mutex won.

**C++, 2 threads, 100,000 iters/thread:**
```
MutexLock:  Volume 200000 ✓  Quantity 1000000 ✓  2.17563e+07 ns
SpinLock:   Volume 200000 ✓  Quantity 1000000 ✓  1.37233e+07 ns
```
Spinlock won.

Both versions correct — volume/quantity match expected in all four runs, no lost updates, no torn reads.

## Correctness notes — the C vs C++ result contradicts itself, and that's the actual lesson
- C run used 4 threads, C++ run used 2 threads — not the same experiment. Thread count changes contention shape; comparing "C says mutex wins" against "C++ says spin wins" as if they're the same test is wrong. They aren't.
- `clock()` in the C harness measures CPU time summed across the process, not wall-clock — on a multi-threaded program this can over- or under-count depending on scheduler behavior. `std::chrono::steady_clock` in the C++ harness measures wall-clock correctly. The two timing methods aren't directly comparable either.
- Core count of the machine wasn't logged in either run. Spinlock behavior is dominated by whether a spinning thread is burning a core that the lock holder needs, or a genuinely idle core elsewhere — this is the single biggest variable in this whole exercise and it's currently unrecorded for both results.
- 0.01s resolution on the C timer (two runs read 0.02 and 0.03 — one tick apart) isn't enough precision to call a winner off one run. C++ side has the same problem in spirit even with nanosecond output — one run each, no averaging, no warm-up discard.
- Conclusion: current results are not evidence spinlocks are "faster in C++ but slower in C." They're evidence the two experiments differ in thread count, timer method, and unknown core count. Rerun both languages with matched thread count, matched core count (log `nproc`/pin threads), `steady_clock`/`chrono` timing in both, and multiple runs averaged before writing any real conclusion in the log.

## Todo next
- Fix the experiment: same thread count, same timing method, same machine, logged core count, both languages, before trusting any "X beats Y" claim
- Upgrade TAS → TTAS in both, re-run, expect the win margin for spinlock to change under contention
- Move to Day 40: deadlock detection — resource allocation graph, cycle detection, lock ordering