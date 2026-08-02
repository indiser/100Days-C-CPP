# Day 18 — Placement Construction `GAME`

## What
Construct objects directly inside raw, pre-allocated memory using placement `new` — no allocation at construction time, arena/buffer owns the bytes. Manually call destructors since placement-constructed objects skip normal delete-driven cleanup. Core building block for object pools, arenas, and anything avoiding malloc on the hot path.

## Files
- `placement.c` — manual "placement" in C: raw `uint8_t arena[]` buffer, `alignas(alignof(Entity))` for correct alignment, cast buffer slice to `Entity*`, init/destroy via plain functions (C has no placement `new`, so this is the manual equivalent — construct-in-place by writing fields directly)
- `placement.cpp` — real placement `new` via `FixedObjectPool<T, Capacity>`: `alignas` byte storage array, `slot_used_[]` bool tracking, `::new (address) T(args...)` for construction, explicit `ptr->~T()` for destruction, index computed via pointer arithmetic `(reinterpret_cast<std::byte*>(ptr) - storage_) / sizeof(T)`
- `Logs.txt` — day log + valgrind run confirming no leaks

## Build

C:
```
gcc placement.c -o placement
./placement
```

C++:
```
g++ placement.cpp -o placement_cpp
./placement_cpp
```

## Design

**No malloc, no free — the arena owns the memory.** Buffer is stack-allocated (`alignas(alignof(Entity)) uint8_t arena[sizeof(Entity) * 4]` in C, `alignas(alignof(T)) std::byte storage_[Capacity * sizeof(T)]` in C++). Construction just writes an object's bit pattern into existing bytes; no heap round-trip.

**Alignment matters or it's UB.** Placing a 4-byte-aligned `Entity`/`Particle` at a misaligned offset is undefined behavior even if it "seems to work." `alignas(alignof(T))` on the storage array guarantees every slot inside it lands correctly aligned — this is the whole reason placement new needs care over `char buf[N]`.

**C has no placement `new` — you fake it.** `placement.c` casts a raw buffer slice to `Entity*` and initializes fields manually via `entity_init()`. No language-level construction step exists; the struct just *becomes* valid once every field is written. Destruction is the same idea in reverse — `entity_destroy()` is a convention, not a language guarantee.

**C++ placement `new` is explicit and separate from allocation.** `::new (address) T(args...)` — the `(address)` form calls `T`'s constructor at an existing address instead of asking for new memory. Pairing destructor call (`ptr->~T()`) with placement new is mandatory: normal `delete` would try to *free* the address, which is wrong since the pool owns it, not the object.

**Object pool reuses slots.** `FixedObjectPool<Particle, 2>` — `allocate()` scans `slot_used_[]` for a free slot, constructs there. `deallocate()` destructs and clears the flag. Freeing `p1` and then allocating `p3` reuses `p1`'s exact bytes — visible in the log: `Particle 1 destroyed` then `Particle 3 created`, no new heap traffic.

**Index-from-pointer trick.** `deallocate()` recovers which slot a pointer belongs to via `(reinterpret_cast<std::byte*>(ptr) - storage_) / sizeof(T)` — pointer arithmetic back to an index, bounds-checked before touching `slot_used_[]`.

## Results
`placement.c` run:
```
C Entity 0: ID=101 POS=(1.0, 2.0)
```
Manual construct/destroy confirmed working on stack-owned memory.

`placement.cpp` run under valgrind (`valgrind --leak-check=full -s ./placement`):
```
Particle 1 created
Particle 2 created
P1 ID: 1, Vel: 15.5
Particle 1 destroyed
Particle 3 created
Particle 2 destroyed
Particle 3 destroyed
...
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
    total heap usage: 2 allocs, 2 frees, 74,752 bytes allocated
ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
Zero leaks, slot reuse confirmed (`p3` takes `p1`'s freed slot). The 2 allocs/2 frees are runtime/iostream startup overhead, not pool activity — pool itself never touches the heap.

## Notes / what broke
Forgetting the explicit destructor call is the classic placement-new bug — object gets silently "leaked" logically (resources like `Particle`'s open handles, if it had any, never release) even though no heap memory is lost, since the pool's own buffer isn't heap-allocated. Valgrind won't catch this class of bug — it only sees malloc/free, not manual construct/destruct pairing.

`alignas` skipped on the storage buffer would still compile and often still "work" on x86 (which tolerates misalignment for most types) — bug stays invisible until it hits SIMD types or a stricter arch. Don't skip it just because it happens to run.

Pool's `deallocate()` silently no-ops on bad pointers/indices (`if (index < Capacity && slot_used_[index])`) — fine for a day exercise, but production code would want to assert or log, not swallow the error.

## Todo next
- Extend `FixedObjectPool` with a free-list (index stack) instead of linear `slot_used_[]` scan — O(1) allocate instead of O(Capacity)
- Add a debug "canary" byte pattern to unused slots, verify nothing writes into unallocated memory
- Try placement new with a type that throws in its constructor — confirm slot doesn't get marked used on a failed construct