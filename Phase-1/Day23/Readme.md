# Day 23 — Object Pools `GAME`

## What
Build fixed-capacity object pools with free-list-in-slot allocation — the pattern behind every entity/particle/bullet system in real game engines. Alloc = pop free-list head, O(1), no search. Free = push slot back onto free-list head, O(1). No separate bookkeeping array for slot state — the "next free" pointer lives inside the dead object's own memory. C way: hand-rolled `Entity_Pool_Alloc`/`Entity_Free` over a flat `PoolNode` array, struct-wrapped union so slot size stays `max(sizeof(Entity), sizeof(void*))` while still carrying an explicit `in_use` tag for double-free detection. C++ way: `ObjectPool<T, Capacity>` — templated, placement-new construct / explicit-destructor destroy, RAII acquire via `unique_ptr` + custom deleter, real support for non-POD `T` unlike the C version.

## Files
- `Entity_ObjPool.c` — fixed-slot free-list pool, `Entity` POD struct, capacity 1028. Core ops: `Entity_Init` (via `__attribute__((constructor))`), `Entity_Pool_Alloc`, `Entity_Free`, `Pool_Reset`. Union-based slot (`entity` / `next` overlap) wrapped in a struct with a `bool in_use` tag alongside.
- `Entity_ObjectPool.cpp` — templated `ObjectPool<T, Capacity>` class. `construct`/`destroy` pair does placement-new + explicit `~T()`, plus `acquire()` returning a `unique_ptr<T, Deleter>` for scope-based auto-release. `in_use_` tracked in a parallel `bool[Capacity]` array rather than packed into the node.
- `ObjPool_w_Freelist.c` — earlier draft, `PoolObject` w/ explicit `next` field (not union) over `Vector3D` payload, used to benchmark pooled alloc vs raw `malloc`/`free` per-object.
- `Logs.txt` — day log, includes the malloc-vs-pool benchmark numbers and the WSL/`time` note.

## Build

C:
```
gcc -O2 Entity_ObjPool.c -o entity_pool -fsanitize=address,undefined && ./entity_pool
gcc -O2 ObjPool_w_Freelist.c -o vec_pool_bench -fsanitize=address,undefined && ./vec_pool_bench
```

C++:
```
g++ -O2 -std=c++20 Entity_ObjectPool.cpp -o entity_pool_cpp -fsanitize=address,undefined && ./entity_pool_cpp
```

## Design

**Free list threaded through dead slots, no separate metadata array (C version's original intent).** `PoolNode` unions `Entity` with a `next` pointer — a dead slot's memory is reused to store the "next free" link, so no parallel array of size/state is needed for the pointer-chasing itself. This is the one insight the whole day hinges on: reuse the space the object already owns instead of paying for bookkeeping beside it.

**`in_use` tag reintroduced despite breaking pure minimalism, on purpose.** Strict free-list-in-slot design has zero room for a safety flag — slot size is exactly `max(sizeof(T), sizeof(void*))`. Adding `bool in_use` to the C `PoolNode` struct grows every slot by (padded) one byte, which is a real, named tradeoff: pure spec-minimal pooling has no double-free protection at all, since a freed slot's contents are entirely overwritten by the `next` pointer and there is nothing left to check against. Chose correctness over minimalism here, explicitly, not by accident.

**Bounds + offset checks replace `assert` for anything touching a caller-supplied pointer.** First drafts used `assert(&(object_pool[i].entity) == entity)` alone — compiles to nothing under `-DNDEBUG`, so a release build would compute a garbage index from a garbage pointer and corrupt the pool silently. Replaced with real `if` checks (`ptr_in_pool`-equivalent range check, then offset-match check) that return `false`/log on failure in every build configuration, not just debug.

**Double-free guard, same shape in both languages.** C: `in_use` bool per slot, checked before re-pushing onto the free list. C++: parallel `in_use_[Capacity]` array indexed by node offset, checked in `destroy()` before calling `~T()` and relinking. Both reject a second free of the same pointer instead of silently corrupting the free list into a cycle (which would otherwise hand the same slot out to two live owners at once — an aliasing bug worse than a crash).

**C++ RAII layer on top of the raw pool.** `acquire()` wraps `construct()` in a `unique_ptr<T, Deleter>` where `Deleter` calls `pool->destroy(ptr)` — object returns to the pool automatically at scope exit, no manual `destroy()` call required at use sites that don't need to hold the raw pointer long-term. Raw `construct`/`destroy` still exposed directly for callers that need manual lifetime control (e.g. an entity that outlives its allocating scope).

**`reset()` must destruct live objects in C++, cannot in C.** C++ `ObjectPool::reset()` walks `in_use_[]` and calls `~T()` on every live slot before relinking the free list — required because `T` may own resources. C's `Pool_Reset` does not and cannot do this generically (`Entity` is POD, so it's safe today), and is commented as safe only for POD payloads — the moment a pooled struct owns a heap resource, the C version needs the same explicit-destruct-before-reset treatment or it silently leaks.

**Pool-vs-malloc benchmark, not just theory.** `ObjPool_w_Freelist.c` ran the same alloc/free workload (up to 100,000 objects/round, 100 rounds) through raw `malloc`/`free` and through the pool. Gap was roughly 1300x (see Logs.txt) — pool version stays O(1) array-index + pointer-swap with zero syscalls and hot cache reuse of a fixed static array, while the malloc path pays libc bookkeeping and likely page-fault cost on first touch each round. Number is real, not cherry-picked — same machine, same workload shape, only the allocator swapped.

## Results
`Entity_ObjPool.c` / `Entity_ObjectPool.cpp` — alloc/free/double-free/exhaustion cycle, capacity 1028:

```
C:   double-free rejected, exhaustion NULL returned at capacity, ASan/UBSan clean
C++: double-free rejected via destroy()==false, RAII handle auto-destroys at scope exit,
     exhaustion returns nullptr at capacity, ASan/UBSan clean
```

`ObjPool_w_Freelist.c` — 100 rounds, up to 100,000 pooled `Vector3D` allocs/frees per round:

```
WithOut Freelist: real 1m21.548s
With FreeList:    real 0m0.061s
```

## Notes / what broke
First C draft (`Entity_ObjPool.c`, pre-fix) used `assert` alone for both the free-list-corruption check and pointer validation — silently compiles to nothing under `-DNDEBUG`, meaning a release build would accept a garbage pointer in `Entity_Free`, compute a garbage index, and either OOB-write or corrupt the free list with zero diagnostic. Replaced with real bounds/offset checks that stay active in every build.

No double-free guard at all in the first two pool drafts (`ObjPool_w_Freelist.c` and first `Entity_ObjPool.c`) — freeing the same pointer twice threads the same slot onto the free list twice, so two live `Alloc()` calls can receive the identical slot and silently alias each other. This is worse than a crash since nothing observably fails at the moment of corruption. Fixed by adding an `in_use` tag checked before every free, in both C and C++ versions, after the same gap was flagged three files in a row — noted here as a pattern, not a one-off miss.

`Pool_Destroy` originally named as if it tears the pool down, but only relinked pointers — no destructor calls, silently safe only because `Entity` happens to be POD. Renamed to `Pool_Reset` and commented explicitly: the moment a pooled struct owns a heap resource (a `char*`, a `std::vector`, anything non-trivial), this function as written would leak it. C++ version doesn't have this trap since `reset()` already destructs every live slot generically.

`std::byte`-based union storage in the C++ pool (`alignas(alignof(T)) std::byte storage[sizeof(T)]`) chosen over `std::aligned_storage_t` from the start, since Day 22's log already flagged that type as deprecated in C++23 — carried the fix forward instead of re-discovering it.

Allocation-table-style tracking (used earlier in the same day's `freelist.c` exercise) deliberately not reused here — object pool's fixed-slot, same-size-every-time design means a per-slot `bool` is strictly enough; a full allocation-record table would be solving a problem (variable-size tracking) this structure doesn't have.

## Todo next
- Add a generation counter (alloc epoch per slot) if double-free detection ever needs to survive slot reuse across many alloc/free cycles with high confidence, not just immediate re-free.
- Re-run `ObjPool_w_Freelist.c` benchmark with `perf stat` instead of `time` to break the 1300x gap down into cache-miss / page-fault / syscall components instead of treating it as one number.
- Extend C++ `ObjectPool` to satisfy the named Allocator requirements loosely (`rebind`, etc.) ahead of Day 27, since this pool is structurally close to what that day needs.