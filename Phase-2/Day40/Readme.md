# Day 40 — Deadlock Detection (Resource Allocation Graph, Cycle Detection via DFS)

## What
Custom mutex wrapper (`my_mutex_t` in C, `DeadlockDetectorMutex` in C++) that tracks a wait-for/resource-allocation graph on every lock/unlock call and runs DFS cycle detection before granting a lock. Three threads each hold one mutex and request the next in a ring (T1: M1→M2, T2: M2→M3, T3: M3→M1) — classic circular-wait setup. If the pending request would close the cycle, the request is denied before the real (`pthread_mutex_t`/`std::mutex`) lock is ever taken, so the physical deadlock never actually forms.

## Files
- `DeadlockDetection.c` — global `Graph` (fixed `Node[64]` + adjacency matrix), one `pthread_mutex_t` (`g_graph.lock`) guarding all graph mutation, `dfs`/`detect_cycle` run under that lock on every `my_mutex_lock` call, 3 `pthread_t` workers on a 3-mutex ring
- `DeadlockDetection.cpp` — `LockGraph` singleton (`unordered_map<uint64_t, unordered_set<uint64_t>>` adjacency), `DeadlockDetectorMutex` class wrapping `std::mutex`, throws `DeadlockException` instead of returning an error code, same 3-thread ring via `std::thread`

## Build
```
gcc -pthread DeadlockDetection.c -o dl_c
g++ -std=c++17 -pthread DeadlockDetection.cpp -o dl_cpp
```

## Run
```
./dl_c
./dl_cpp
```

## Design
- Detection happens **before** the real lock call, not after — request adds a "waiting" edge, checks for cycle, and if found, removes the edge and bails without ever calling `pthread_mutex_lock`/`native_mutex_.lock()`. This is what actually prevents the deadlock rather than just detecting one that already happened — only the thread that would *close* the cycle gets denied; the other two already hold their real locks and proceed normally.
- Edge semantics: `thread -> mutex` means "waiting for", `mutex -> thread` means "held by". Cycle in this directed graph = circular wait = deadlock, standard resource-allocation-graph model.
- C: graph guarded by a single `pthread_mutex_t`, fixed-size `Node[MAX_NODES]` + `int adj[MAX_NODES][MAX_NODES]` — no dynamic growth, everything serialized through one lock.
- C++: same idea, `unordered_map`/`unordered_set` instead of fixed matrix, `std::lock_guard` on the registry mutex, cycle reported via exception instead of sentinel return value — more idiomatic, same underlying algorithm.
- `sleep(1)`/`sleep_for(100ms)` between "got lock" and "requesting next" is load-bearing — it's what staggers the three requests enough for the graph to reach the full 6-edge cycle before any thread physically blocks on the third lock.

## Results

**C, 3 threads, ring M1→M2→M3→M1 (5 runs):**
```
T1 got M1
T2 got M2
T3 got M3
T3 requesting M1...
T2 requesting M3...
T1 requesting M2...
[DEADLOCK DETECTED] Thread <tid> requesting Mutex 102 creates cycle!
T3 got M1
T2 got M3
```
5/5 runs terminated clean, exactly one denial per run, no hang.

**C++, same ring (5 runs):**
```
T1 got M1
T2 got M2
T3 got M3
T1 requesting M2...
T2 requesting M3...
T3 requesting M1...
[DEADLOCK DETECTED] Thread <hash> requesting Mutex 101 causes cycle!
T2 got M3
T1 got M2
```
5/5 runs terminated clean, one `DeadlockException` per run, no hang.

Both versions: correct cycle detection, program always exits, denial always lands on whichever thread's request happens to close the loop (order varies run to run — that's scheduler nondeterminism, not a bug).

## Correctness notes — half-fixed bugs still in the code
- C added a `get_thread_id()` helper (`(uint64_t)(uintptr_t)pthread_self()`, correct — no truncation) but **never calls it**. `my_mutex_lock`/`my_mutex_unlock` still use `int tid = (int)pthread_self();` on line 100 and 127. Dead code sitting next to the live bug it was meant to replace — worse than not fixing it, because it looks fixed on a skim.
- C++ patched `worker_1` with an `m1_acquired` flag so the catch block only unlocks a mutex it actually locked. `worker_2` and `worker_3` were **not** patched — same unconditional `mX.unlock()` in their catch blocks, same UB landmine if `add_request` ever throws on the *first* lock instead of the second. Doesn't fire in this exact 3-thread ring (cycle only ever completes on the second lock in the chain), but the class is now inconsistent with itself — one caller respects the invariant, two don't.
- Global lock (`g_graph.lock` / `registry_mutex_`) still serializes every lock/unlock system-wide through a full graph rescan. Fine at 3 threads, doesn't survive contact with real concurrency — same note as Day 39's spinlock, not addressed here either.
- `MAX_NODES 64` overflow now `exit(EXIT_FAILURE)`s instead of silently corrupting memory — that part's a real, complete fix.

## Todo next
- Finish the thread-id fix in C: delete lines 100/127's `(int)pthread_self()`, actually call `get_thread_id()`
- Apply the `_acquired` flag pattern to `worker_2`/`worker_3` in C++ so all three are consistent, not just the one that got noticed
- Replace single global graph lock with finer-grained locking or a lock-free structure, benchmark contention at 10+ threads
- Move to Day 41: raw sockets + custom binary protocol