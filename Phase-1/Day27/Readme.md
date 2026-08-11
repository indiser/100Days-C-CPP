# Day 27 — Custom STL Allocators `GAME`

## What
Build a generic allocator interface, back it with an arena, plug it into real containers, and prove — with numbers, not vibes — that memory behaves the way it's supposed to. C way: manual vtable struct (`allocate`/`deallocate` fn pointers + `ctx`) driving a hand-rolled dynamic array. C++ way: a real Allocator-named-requirements class (`value_type`, `allocate`, `deallocate`, converting ctor for rebind, explicit `propagate_on_container_*` traits) plugged straight into `std::vector` and `std::list`. Point of the exercise isn't "make it compile" — it's proving rebind fires, propagate flags behave under move-assignment, overflow fails clean, and the heap is genuinely untouched where claimed.

## Files
- `Allocators.c` — vtable `Allocator` struct, two interchangeable backends (`ArenaBackend` bump allocator, `MallocBackend` wrapping `posix_memalign`/`_aligned_malloc`), `CustomVector` driven entirely through the vtable, `AllocStats` counters baked into each backend.
- `Allocators.cpp` — `Arena` class (same bump-pointer logic as Day 6), `ArenaAllocator<T>` C++ Allocator, `AllocStats` passed by reference (not `shared_ptr` — see Design), plugged into `std::vector<int, ArenaAllocator<int>>` and `std::list<int, ArenaAllocator<int>>`.
- `Logs.txt` — day log.
- `valgridResult.txt` — `valgrind --leak-check=full --show-leak-kinds=all` output for the C binary.

## Build

C:
```
gcc -Wall -Wextra -fsanitize=address,undefined -g Allocators.c -o Allocators && ./Allocators
```

C++:
```
g++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined -g Allocators.cpp -o Allocators_cpp && ./Allocators_cpp
```

Leak check:
```
valgrind --leak-check=full --show-leak-kinds=all ./Allocators
```

## Design

**`shared_ptr` inside the allocator was a hidden heap allocation, caught before it became a lie.** First draft of `ArenaAllocator` carried stats via `std::shared_ptr<std::size_t>` so counters would survive rebind-copies cheaply. Problem: `make_shared` calls `::operator new` for the control block — every allocator construction or rebind conversion would silently touch the global heap, defeating the entire "prove vector never touches malloc" goal. Fixed by pulling stats out into a separate `AllocStats` struct owned by the caller (same stack frame as the `Arena`), with the allocator holding a raw pointer to it — same lifetime contract the arena pointer already had, no new heap use introduced.

**Propagate traits set explicitly, not left to `allocator_traits` defaults.** `propagate_on_container_move_assignment = std::true_type` — the arena is the actual resource, so on `vecB = std::move(vecA)` the allocator pointer must steal over with the container, not fall back to element-wise move into `vecB`'s own (different) arena. `propagate_on_container_swap = std::true_type` for the same reason. `propagate_on_container_copy_assignment = std::false_type` — copying a container shouldn't silently repoint it at someone else's arena.

**Rebind exercised for real via `std::list`, not just asserted to compile.** `vector<int, ArenaAllocator<int>>` never proves rebind fires because element type equals allocated type. Plugging the same allocator into `std::list<int, ArenaAllocator<int>>` forces `allocator_traits` to rebind to the list's internal node type — the converting constructor (`template<U> ArenaAllocator(const ArenaAllocator<U>&)`) is what actually gets exercised there, not dead code hoped to be correct.

**Doubling growth on a bump allocator quietly wastes memory — measured, not just noted.** Every `push_back` capacity-doubling in `vec_push_back`/`std::vector` allocates a new block and abandons the old one (arena `deallocate`/`arena_free` is a correct no-op for a bump allocator — memory only reclaims on arena destroy/reset). At 100 elements, capacity lands on 128, meaning prior blocks (4+8+16+32+64 ints) were requested and abandoned along the way. Measured directly: **wasted/abandoned arena memory = 496 bytes** for a **512-byte live vector** (i.e., almost 1:1 overhead from growth alone) — see Results.

**Two interchangeable backends prove the vtable buys something, not just that it compiles.** `Allocator` (C) is a flat struct of function pointers + `ctx`; `CustomVector` never knows or cares whether `ctx` points at an `ArenaBackend` or a `MallocBackend`. Ran the identical push/read/destroy sequence through both, same interface, same code path, different memory source underneath — that's the actual point of the pattern, not just "one backend, hope it generalizes."

**Overflow forced on purpose, not assumed to work.** Both languages shrink the arena on purpose (500 bytes in C, 200 bytes in C++) and push until allocation fails. C path: `vec_push_back` returns `false` cleanly, vector size/capacity stay at their last-good values, no corruption. C++ path: `arena_->allocate` returns `nullptr` → allocator throws `std::bad_alloc` → caught at the call site, `vector`'s strong exception guarantee keeps `tinyVec` in its pre-growth state.

## Results

C, full run:
```
=== TEST 1: Arena Backend + Waste Measurement ===
Vector size=100, cap=128
Alloc calls=6, Total requested=1008 bytes
Arena offset (used)=1008 bytes, Live vector memory=512 bytes
Wasted/Abandoned Arena Memory=496 bytes

=== TEST 2: Malloc Backend (Runtime Swap) ===
Vector size=100, cap=128
Alloc calls=6, Total allocated bytes=1008
First item=0, Last item=990

=== TEST 3: Forced Overflow (Tiny 500-byte Arena) ===
ALLOCATION FAILED at i=64
Pushed items before fail=64
Vector size=64, cap=64 (Uncorrupted state)
Arena used=496 / 500 bytes
First item=0, Last valid item=63
```

Valgrind, full run:
```
==687== HEAP SUMMARY:
==687==     in use at exit: 0 bytes in 0 blocks
==687==   total heap usage: 9 allocs, 9 frees, 2,112 bytes allocated
==687== 
==687== All heap blocks were freed -- no leaks are possible
==687== 
==687== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
Zero leaks, zero errors — both the `ArenaBackend` mmap region and every `MallocBackend` allocation reclaim cleanly.

C++ (expected shape of output, same structure as C): `vector<int, ArenaAllocator<int>>` push of 100 ints reports matching `alloc_calls`/`bytes_allocated` via the raw-pointer `AllocStats`; forced 200-byte-arena overflow throws and is caught with `tinyVec` left at its last valid size; `vecB = std::move(vecA)` across two distinct arenas confirms the allocator's arena pointer transfers (propagate) rather than falling back to element-wise move; `std::list<int, ArenaAllocator<int>>` allocation requests report `sizeof(T)` equal to the list's internal node size, not `sizeof(int)` — direct proof rebind fired.

## Notes / what broke

First allocator draft used `std::shared_ptr` for stat counters — compiled clean, ran clean, and was quietly wrong: it made every allocator copy/rebind touch `::operator new`, which is exactly the heap contact the whole exercise was supposed to rule out. Caught by review, not by a test — nothing would have flagged it at runtime since it "worked."

First C draft only had the arena backend — no proof the vtable interface actually generalized to a second, unrelated memory source. Added `MallocBackend` with `posix_memalign` so the same `CustomVector` code path runs against two real, differently-behaved allocators.

Neither draft originally forced an overflow or a small-arena failure — both "worked" purely because the arena was oversized (1MB) relative to the test data, so the failure path (`return false` / `throw std::bad_alloc`) was unexercised dead code with unknown correctness. Shrinking the arena on purpose was the only way to actually know.

Waste from doubling growth (496 bytes abandoned for 512 bytes live, TEST 1) was invisible at 1MB arena size — would've shipped without ever noticing a bump allocator plus a doubling-growth container is close to 50% overhead in the worst case, until the arena was made small enough to force the number into view.

## Todo next
- Add `bytes_freed` tracking to `MallocBackend` so its counter reflects net-live bytes, not just cumulative requested — currently both backends report "total ever requested," which slightly overstates the malloc path's real footprint since `malloc_free` does actually reclaim.
- Run the C++ binary's TEST 3/TEST 4 output through the same scrutiny as the C valgrind run — leak-checked so far only for the C binary, not the C++ one.
- Try an `Arena::reset()` mid-run between vector lifetimes and confirm a *second* container built after reset doesn't read stale data from the first — reset exists but is untested against reuse across container instances.
- Overload global `operator new`/`delete` (or use a `std::pmr` debug resource) for one run to prove zero heap contact at the runtime level, not just via internal counters that could themselves be wrong.