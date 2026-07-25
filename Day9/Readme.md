# Day 9 — Buddy Allocator (Phase 1) `EMBED`

## What
Buddy memory allocation algorithm — power-of-two block splitting, free lists per order, buddy-address coalescing on free. C and C++ versions, C++ wrapped as a standard-library-compatible `Allocator` and plugged into `std::vector`.

## Files
- `buddy_alloc.c` — buddy allocator, fixed pool, min/max order derived from pool size + min block size, stress test with overlap + full-coalesce checks
- `buddy_alloc.cpp` — same core logic in `BuddyAllocatorRaw`, plus `BuddyAllocator<T>` template satisfying C++ Allocator requirements, sanity-tested via `std::vector<int, BuddyAllocator<int>>`
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -O2 -fsanitize=address,undefined buddy_alloc.c -o buddy_alloc
```

C++:
```
g++ -O2 -fsanitize=address,undefined buddy_alloc.cpp -o buddy_alloc_cpp
```

## Design

**Block header.** Each block, free or allocated, carries a `BlockHeader` (`next`, `prev`, `order`, `is_free`) at its start. Payload pointer returned to caller is `block + 1` — header sits invisibly before user memory, same trick as malloc implementations.

**Order-indexed free lists.** `free_lists[i]` holds a doubly-linked list of free blocks at order `min_order + i`. Allocation searches upward from the target order for the first non-empty list, splits it down one order at a time until reaching target size, pushing the unused buddy half into the free list at each step.

**Buddy address via XOR.** On free, buddy's address is computed as `offset ^ block_size` relative to pool base — the defining trick of buddy allocation, works because buddies always differ in exactly one bit at their size's boundary. Coalescing walks up merging with the buddy while it's free and the same order, stops on first non-mergeable buddy.

**Bounds assert.** `MAX_ORDERS` fixed at compile time; `buddy_init`/constructor now asserts the computed order range fits inside it, instead of silently overrunning `free_lists[]` if a bigger pool or smaller min-block-size is passed in later.

**C++ Allocator wrapper.** `BuddyAllocator<T>` holds a raw pointer to `BuddyAllocatorRaw`, implements `allocate`/`deallocate`/`operator==`/`operator!=`, satisfies enough of the named-requirements set for `std::vector` to accept it as its allocator template argument. `deallocate` ignores the size parameter on purpose — block order in the header already encodes size, comment left in code explaining why so it doesn't read as a bug.

## Testing

Original version only alloc'd two blocks, freed them, and printed "success" — that's not proof, just a vibe check. Replaced with:
- **Stress test**: 5000 rounds of random alloc/free across 64 slots, sizes 1–256 bytes, pool size 64KB. Every successful alloc checked against all live allocations for address-range overlap before being trusted.
- **Memory touch**: every allocated block is `memset` immediately — turns any off-by-one or bad split into an ASan crash instead of a silent pass.
- **Coalesce proof**: after freeing everything, asserts exactly one block sits in the top-order free list and every lower-order list is empty — proof the whole pool merged back to a single root block, not an assumption.

## Notes / what broke

**Silent OOB risk on `MAX_ORDERS`** — original code hardcoded the free-lists array size with no check against the actual order range computed from pool size and min block size. A bigger pool or smaller min-block-size later would silently overrun the array. Fixed with an assert in init/constructor.

**No proof of correctness initially** — first pass "worked" on a two-alloc, two-free happy path only. That path can't catch partial-split merge bugs or wrong buddy math. Real bugs live in the messy random-order case, which the stress test now covers.

**`deallocate` unused size param** — looked like a bug on first read since the C++ Allocator interface hands it a size that's silently dropped. Left a comment: header's stored order makes the size argument redundant for this allocator.

## Known limitation
`BuddyAllocatorRaw` is not thread-safe — no locking around free-list mutation. Fine for today's single-threaded sim, but flagged here so it isn't reused as-is once Phase 2 concurrency work starts.

## Phase 1 status: day 9 closed
Stress test passes: no overlaps across 5000 random alloc/free rounds, full coalesce back to single root block verified by assertion, not by eyeball.

## Todo next
- Day 10 — Cache-friendly layout, `GFX` domain: particle system AoS vs SoA benchmark.