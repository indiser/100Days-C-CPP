# Day 25 — Paged Storage `DB`

## What
Build a SQLite-pager-style storage engine — fixed-size pages, page cache with LRU eviction, pin-counted access so a page in use can never get evicted out from under you, and a free list so deleted pages get reused instead of growing the file forever. C way: manual malloc'd `Page` nodes, hand-rolled doubly-linked LRU list + hash map for O(1) lookup, `pread`/`pwrite` for disk I/O. C++ way: `BufferPoolManager` — fixed frame pool (`std::vector<Page>`), `std::list`+`std::unordered_map` LRU, and a move-only RAII `PageGuard` that pins on fetch and auto-unpins on scope exit.

## Files
- `pager.c` / `pager.h` — hash map + doubly-linked LRU list, page 0 reserved as `DatabaseHeader` (page_size, total_pages, free_list_head), `pager_alloc_page`/`pager_free_page` free-list logic, dirty/clean tracked via `pwrite_count`.
- `main.c` — 4-test suite: clean-vs-dirty eviction metric, free-list reuse, mid-write crash safety, random thrashing.
- `BufferPoolManager.hpp/.cpp` — frame-pool version of the same design, `page_table_` maps page_id→frame_id instead of chasing raw pointers.
- `PageGuard.hpp` — RAII pin wrapper, move-only, unpins in dtor.
- `main.cpp` — same 4 tests ported, plus a frame-lock assertion the C suite couldn't express.
- `Logs.txt` — day log.

## Build

C:
```
gcc -Wall -Wextra -fsanitize=address,undefined -g pager.c main.c -o pager_test && ./pager_test
```

C++:
```
g++ -std=c++17 -Wall -Wextra -fsanitize=address,undefined -g BufferPoolManager.cpp main.cpp -o cpp_pager_test && ./cpp_pager_test
```

(CMake build was written explicitly for WSL — `pread`/`pwrite` aren't available on native Windows, ran everything through WSL for this one.)

## Design

**Page 0 reserved as metadata header, not treated like any other data page.** First draft skipped this — no reserved header page at all, no free-list-head storage anywhere. Retrofitted `DatabaseHeader{page_size, total_pages, free_list_head}` into page 0's raw bytes, initialized on first open (`file_len == 0` check), flushed immediately so the header is durable even before the first real `pager_close`.

**Pin count is the whole point of the exercise, not an afterthought.** `pager_evict`/`FetchPage`'s eviction path walks the LRU tail and explicitly skips any page with `pin_count > 0`. Without this, cache pressure mid-operation silently evicts a page someone still holds a pointer into — that's a live use-after-free, not a hypothetical. C++ side wraps this in `PageGuard` so the discipline is enforced by the type system instead of by remembering to call `unpin` — same guarantee, harder to violate.

**Free list stored inside freed pages themselves, not a separate structure.** `pager_free_page` zeroes the target page and writes the *current* free-list-head page_id into its first 4 bytes, then makes itself the new head. `pager_alloc_page` reverses this — pop the head, read its stored `next_free` back out, advance the header pointer. No extra allocation, no separate free-list array; the freed pages' own bytes are the linked list.

**Dirty vs clean tracked explicitly, verified with a real counter, not just asserted by inspection.** `pwrite_count_`/`pwrite_count` increments only inside `pager_flush`/`FlushPage`, only when `is_dirty` was true. Both test suites fetch a batch of pages, mark half dirty and half clean, force eviction of all of them, and assert the write count only moved by the dirty count. Without this, "the cache works" is just a vibe — this is what actually proves clean evictions skip disk I/O instead of always doing buffered I/O with extra steps.

**Crash safety tested by literally not calling the normal teardown path.** `pager_close`/`~BufferPoolManager` both flush on the way out — that alone proves nothing about crash safety, since a clean shutdown always succeeds. Added `pager_destroy_no_flush`/`DestroyNoFlush()` as an explicit "pretend the process died" escape hatch: write dirty data, unpin it, skip the flush, tear down, reopen from disk, assert the uncommitted write is *not* there. This is the test that actually earns the word "durable."

**C++ frame pool (`std::vector<Page>` + free-frame list) instead of malloc'd nodes per page.** More disciplined than the C version — whole pool allocated once up front, `page_table_` maps page_id to a stable frame index instead of chasing raw pointers, no malloc/free churn per fetch/evict cycle. This is closer to how a real buffer pool manager is built than the C side's per-page malloc.

**`GetFrameId()` added specifically to make the pin-vs-eviction test assert something real.** First cut of `TestPinPreventsEviction` only checked `FetchPage() != nullptr` after forcing eviction pressure — that proves the call didn't crash, not that pinning worked, since a silently-re-read-from-disk page would also return non-null. Fixed by exposing the page's frame index and asserting it's identical before and after the eviction round, plus asserting the *unpinned* neighbor pages did get evicted. Proves both the positive and negative case in one test instead of a fuzzy "seems fine."

## Results

All 4 tests, both languages, ASan+UBSan clean:
```
[PASS] Clean vs Dirty Eviction Metric Assert
[PASS] Page 0 Header Free List Allocation Reuse
[PASS] Mid-Write Crash Safety (ASan Clean)
[PASS] Random Thrashing Stress Test Passed
```
```
[PASS] C++ Frame-Lock Pin Eviction Safety Assert
[PASS] C++ Clean vs Dirty Pwrite Count Metric
[PASS] C++ Random Thrashing Stress Test Passed
[PASS] C++ Mid-Write Crash Safety (Unflushed Dirty Loss Confirmed)
```

## Notes / what broke

First C draft had no free list at all — spec called for it explicitly, first pass shipped without it. Caught on review before it got called "done," not by a failing test, since nothing in the original suite exercised alloc/free/reuse.

First crash-safety test in C simulated "process death" with `close(fd); free(pager);` directly in the test body instead of a dedicated teardown function — worked, but leaked the still-cached `Page` nodes (asan flagged 8272 bytes). Fixed by adding `pager_destroy_no_flush`/`DestroyNoFlush()` as a real API surface instead of hand-rolling teardown logic inside a test function. Lesson: a "simulate a crash" test still has to clean up its own harness state even when the thing it's simulating doesn't.

First C++ port of the test suite dropped three of the four C tests — ported the class and the RAII wrapper faithfully but only wrote one matching test (`TestRAIIPinScope`), leaving pin-vs-eviction, dirty/clean metrics, and random thrashing completely uncovered on the C++ side. Prettier code with weaker proof than the "toy" C version until caught and backfilled — parity between C and C++ test coverage isn't automatic just because the class compiles and one smoke test passes.

First `TestPinPreventsEviction` asserted `!= nullptr` only — technically passed, proved nothing about pinning specifically. Needed `GetFrameId()` exposed before the assertion was actually meaningful.

## Todo next
- No test proves header page (page 0) survives eviction pressure when it's mixed into a full cache alongside >10 unique competing pages — alloc/free always fetch+unpin page 0 tightly so it can't currently break, but that's an argument from code reading, not from a test.
- Free-list reuse only tested for a single free→alloc cycle. Should chain 3–4 frees and confirm LIFO reuse order holds across multiple rounds, not just one.
- No test exercises the "all pages pinned, `pager_evict` returns NULL" path — it exists in both C and C++, never triggered by either suite.