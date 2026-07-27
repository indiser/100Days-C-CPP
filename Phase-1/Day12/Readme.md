# Day 12 — Smart Pointer Internals (Phase 1) `SYS`

## What
Reference counting, RAII, move semantics, operator overload (`*`, `->`). Built manual refcounted pointer in C (no smart pointers built into language), and hand-rolled `unique_ptr`/`shared_ptr` clones in C++.

## Files
- `refCountPointer.c` — refcounted box (`RefBox`), manual retain/release
- `uni_ptr.cpp` — `MyUniquePtr`, move-only, deleted copy
- `share_ptr.cpp` — `MySharedPtr`, control block, copy bumps refcount
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -Wall -Wextra -O2 refCountPointer.c -o refbox
```

C++:
```
g++ -Wall -Wextra -O2 uni_ptr.cpp -o uniptr
g++ -Wall -Wextra -O2 share_ptr.cpp -o sharedptr
```

## Design

**RefBox (C).** No language support for smart pointers in C, so refcounting done manually — `refbox_create` sets count 1, `refbox_retain` on share (count++), `refbox_release` on drop (count--, free at 0). Caller responsible for calling retain/release correctly — no compiler enforcement, unlike C++ RAII.

**MyUniquePtr.** Single owner. Copy ctor/assign deleted — compiler blocks accidental double-ownership at compile time (this is the whole win vs the C version). Move ctor/assign steal the raw pointer, null the source — ownership transfers, no refcount needed since only one owner ever exists.

**MySharedPtr.** Separate `ControlBlock<T>` holding raw ptr + refcount, allocated once, shared across all copies. Copy ctor/assign bump refcount. Destructor releases — decrements, frees object + control block at 0. `operator*`/`operator->` forward through the control block so it behaves like a real pointer.

## Known limitation
- **No `weak_ptr` built.** Skipped — this is the actual gap. `weak_ptr` is where shared_ptr's control-block design earns its keep (breaking A↔B ownership cycles). Control block already separated from the object in `MySharedPtr`, so adding a weak count next to the strong count is the natural next step — not done yet.
- `MySharedPtr` has no move ctor/assign — every transfer goes through copy + refcount bump/drop, even when the source is a temporary that doesn't need the bump. Wasted atomic-adjacent work.
- Not thread-safe — refcount is plain `int`, no atomic. Fine for today's single-threaded scope, but worth naming since yesterday was entirely about this exact class of bug.
- `MyUniquePtr` has no `reset()`, no `release()` (the ownership-relinquish kind, distinct from `MySharedPtr::release()`), no `operator bool`. Minimal but incomplete vs real `unique_ptr`.

## Notes / what broke
- Logs from today are thin — "pretty straightforward," "nothing really broke." Worth being honest: that's a sign the day was played safe, not a sign the material is actually easy. Real `shared_ptr` involves `weak_ptr`, `enable_shared_from_this`, atomic refcounts, aliasing constructor — none of that was touched.

## Todo next
- Build `weak_ptr` — same control block, add weak count, `lock()` returns `MySharedPtr` only if strong count > 0
- Add move semantics to `MySharedPtr`
- Make refcount `std::atomic<int>` and stress-test with real shared_ptr under threads — direct callback to Day 11's lock-free work
- Day 13 — Binary serialization, `TRADE`: struct packing, endianness, manual byte buffers, FIX-like market-data serializer