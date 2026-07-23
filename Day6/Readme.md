# Day 6 — Arena Allocators (Phase 1 open) `GAME`

## What
Bump-pointer arena allocator, C and C++ versions. Backed by real OS virtual memory (`VirtualAlloc` on Windows, `mmap` on POSIX), not `malloc`. Goal: understand pointer-bump allocation, manual alignment, and O(1) bulk free — then expose the C++ version as a pluggable `Allocator` usable with `std::vector`.

## Files
- `arena.c` — C arena: `ArenaInit`, `ArenaAlloc(numElem, elemSize, align)`, `ArenaResetPointer`, `ArenaDelete`
- `arena.cpp` — C++ arena (`Arena` class) + `ArenaAllocator<T>` wrapper satisfying the C++ Allocator named requirements, plugged into `std::vector<int, ArenaAllocator<int>>`
- `Logs.txt` — raw notes from the day

## Build

C (Windows):
```
cl arena.c
```
or with clang/MSYS2:
```
clang arena.c -o arena.exe
```

C++ (Windows):
```
cl arena.cpp
```
or:
```
clang++ -std=c++17 arena.cpp -o arena.exe
```

## Design

**Backing memory.** Both versions reserve+commit memory directly via `VirtualAlloc` (Windows) instead of `malloc`. Originally wrote against `mmap`/`sys/mman.h` per the syllabus topic list, but no POSIX layer on this machine — switched to `VirtualAlloc` as the Windows equivalent. Conceptually same operation (ask OS for a raw page-backed region), different API.

**Alignment.** Both allocators round the bump pointer up to the requested alignment before handing out memory, computed from the *absolute* pointer address (not the offset alone) so alignment is correct regardless of where the arena's base happens to sit in memory:
```c
uintptr_t padding = (~totalOffset) & (alignSize - 1);
```
C++ version does the equivalent with `(current + align - 1) & ~(align - 1)`.

**Reset vs free.** `reset()`/`ArenaResetPointer` just zero the offset — O(1), no per-object destructor calls, no memset of the buffer (removed an earlier version that memset the whole buffer on reset, which defeated the point of a bump allocator). Matches the ritual: arena's whole value proposition is bulk O(1) reclaim, not fine-grained free.

**C++ Allocator interface.** `ArenaAllocator<T>::deallocate` is a deliberate no-op — arenas don't support individual free, only bulk reset. `allocate()` throws `std::bad_alloc` on exhaustion so it composes correctly with STL container exception expectations. Rebind constructor (`template<U> ArenaAllocator(const ArenaAllocator<U>&)`) lets one arena back containers of different element types.

## Results
```
used: 400 bytes
after reset: 0 bytes
```
100 `int`s pushed through `std::vector` backed by the arena — matches expected `100 * sizeof(int)`, no padding needed since `alignof(int)` never misaligns a zero-offset start. Reset correctly drops usage to 0 without touching capacity or backing memory.

## Notes / what broke

**First pass copied a reference implementation** instead of building from the "topics to learn first" list directly — a full reserve/commit/chain/scratch-arena system (mr4th-style). Recognized this wasn't "my day 6," discarded it, rebuilt from scratch using only the concept, not the code.

**Power-of-two check was silently broken** in the first from-scratch attempt: `(alignSize & (alignSize - 1)) == 1` never actually triggers for non-power-of-two inputs — off-by-comparison bug, fixed to `!= 0`.

**Unguarded macro** — `KB(x) x * 1024` (no parens) — would break under `KB(1+1)`. Fixed to `((x) * 1024)`. Didn't bite in practice since only ever called with a bare literal, but wrong to leave in.

**mmap'd memory allocated then discarded** — an early version called `mmap`/reserved a pointer, then built the arena's actual buffer from a separate `malloc` call, leaking the reserved region and defeating the entire point of the exercise (using real OS-backed memory instead of the general-purpose allocator). Fixed to use the reserved pointer directly as the buffer.

**No NULL-check on alloc failure** in `main()` — silent NULL-deref risk if the arena fills up. Added an explicit check after `ArenaAlloc`.

**No `sys/mman.h` on this machine** — Windows-only environment, no WSL used for this day (unlike Day 5). Swapped to `VirtualAlloc`/`VirtualFree` directly rather than papering over with an unused POSIX branch never compiled or tested.

## Takeaway
Arena's actual value is two things working together: O(1) bulk reset (no per-object bookkeeping, no leak risk since nothing is ever individually forgotten) and mechanical sympathy from linear/contiguous allocation. The C version makes both costs and mechanics explicit — raw pointer arithmetic, manual alignment, manual OS calls. The C++ version hides the pointer arithmetic behind `Allocator` conformance so the *same* arena can silently back any standard container — the abstraction buys ergonomics, costs nothing extra here since `deallocate` was already a no-op.

## Phase 1 status: day 6 closed
C and C++ arenas built, compiled, and run — bump-alloc, aligned, backed by real virtual memory, C++ side pluggable into `std::vector`. Both bugs found in earlier drafts (broken pow2 check, discarded mmap region) fixed and verified before closing.

## Todo next
- Day 7 — alignment & padding, `TRADE` domain: cache-aligned market-tick struct via `alignas`/`alignof`, false sharing. Already did half this thinking today (manual alignment math) — carry it forward instead of relearning from zero.
- Revisit Day 5's unresolved bug-trap fuzz test when back in a Linux/WSL environment (blocked on tooling, not skipped by choice).