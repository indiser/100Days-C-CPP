# Day 33 — Lock-Free Stack `TRADE`

## What
Build a lock-free stack two ways and force both under adversarial concurrent load. Syllabus called for a lock-free *queue*; swapped to a *stack* since the queue was already built on a prior day — not to dodge difficulty, but because a stack surfaces a nastier version of the same failure mode. Treiber-stack push/pop hits the same head pointer repeatedly, which makes the ABA problem far sharper than it typically is on a queue.

C way: manual Treiber stack with a real `pop()`, tagged 128-bit pointers (`CMPXCHG16B`) to defeat ABA at the CAS level, plus hazard pointers to close the use-after-free window tagging alone doesn't cover. C++ way: a different, valid design — bulk `steal()` via unconditional `exchange()` instead of single-node `pop()`, which sidesteps the ABA/reclamation problem structurally rather than solving it. Both stress-tested with 10 producers / 10 consumers, 100k nodes, under TSan and ASan+UBSan.

## Files
- `treiber_stack.c` — Treiber stack with tagged-pointer CAS (`TaggedPtr{ptr, tag}` packed into `__int128`, compare-swapped as one 128-bit unit) to fix ABA on `pop()`, plus a hazard-pointer scheme (`g_hazard[]`, thread-local slot registration, `retire_node`/`retire_sweep`) to fix the separate use-after-free that tagging alone left open. Single-node `push`/`pop`, full MPMC stress test with correctness check (pushed == consumed, stack empty at exit).
- `TrieberStack.h` / `TrieberStack.cpp` — C++ `TreiberStackWithStealMPMC<Node>` template. `push()` is a standard CAS-loop link (no ABA hazard — push doesn't depend on head's *history*, only its current value, so a stale-then-reused address is still structurally correct to link against). `steal()` is an unconditional `exchange(nullptr)` that rips off the whole chain atomically — no read-then-CAS gap, so no ABA vector and no reclamation problem to solve at all.
- `Makefile` — builds the C version (`tsan`/`asan`/`release` targets, requires `-mcx16 -latomic` for the 128-bit CAS).
- `Makefile.cpp` — builds the C++ version (`tsan`/`asan`/`release` targets, requires `-std=c++20` for `std::jthread`; no `-latomic` needed since the C++ CAS is pointer-width, not 128-bit).
- `Logs.txt` — day log.

## Build

C:
```
make -f Makefile run-test    # TSan + ASan/UBSan, 10 producers / 10 consumers, 100k nodes
make -f Makefile run-bench   # O3 release run
```

C++:
```
make -f Makefile.cpp run-test
make -f Makefile.cpp run-bench
```

## Design

**Tagging fixes the CAS match, not the dereference.** The classic ABA sequence — thread A reads `head=X`, gets preempted, thread B pops X and Y and pushes a *new* node that the allocator happens to place back at address X, thread A resumes and its stale CAS succeeds because the pointer matches — is what the 64-bit tag defeats. Every successful push or pop increments the tag, so even if the pointer address repeats, the packed `(ptr, tag)` 128-bit word never does. The CAS compares the whole word, so a spurious pointer match alone can't fool it.

**Tagging is not memory reclamation.** First TSan run on the tagged-only version caught a real heap-use-after-free: `pop()` reads `head->next` *before* the CAS, and nothing stops another thread from `free()`-ing that exact node in the gap. The tag protects the swap; it does nothing for the read. This is the actual distinction the syllabus notes were pointing at — ABA-the-CAS-problem and use-after-free-the-reclamation-problem are separate bugs with separate fixes, not one problem with two names, and now there's a caught stack trace proving it rather than just the claim.

**Hazard pointers close the reclamation window.** Before dereferencing a node, a thread publishes it into a global hazard slot (`memory_order_release`), re-validates `head` hasn't moved since (catches the free-before-publish race), then it's safe to read. `retire_node()` checks all hazard slots before freeing; anything currently hazardous goes on a mutex-protected retire list instead, swept periodically once it's no longer anyone's target.

**Second TSan run caught a second, self-inflicted bug.** The first hazard-pointer implementation reused `node->next` as the retire-list link field — but `->next` is exactly the field a hazard-protected reader might still be walking. Marking a node hazardous correctly stopped it from being `free()`-d, but the retire-list bookkeeping was still mutating that node's live `->next` field underneath a concurrent reader. Fixed by adding a dedicated `retire_next` field so retire-list linkage never shares memory with stack linkage. The lesson isn't "hazard pointers work now" — it's that even while actively defending against a known bug class, it's trivial to introduce an unrelated race by overloading one struct field for two purposes, and the only thing that caught it was actually running the sanitizer again after the "fix," not eyeballing the diff.

**`steal()` avoids all of the above by construction.** `exchange(nullptr, acquire)` unconditionally swaps the whole chain out in one atomic op — there's no read-then-CAS window for a stale pointer to sneak back into, so no ABA vector exists on the pop side, and no node is ever concurrently readable-and-freeable because ownership transfers atomically and completely. This is a legitimate production pattern (batch-drain / work-stealing dequeue primitive), not a workaround. The trade-off: it can't do a fine-grained single-item pop, and whichever consumer wins the exchange gets the entire pending chain, which is a real design constraint if per-item ordering or load-balancing across consumers matters.

## Results
C (`treiber_stack.c`, `make run-test`):
```
--- TSan (thread + undefined) ---
Target:          100000
Total pushed:    100000
Total consumed:  100000
Stack empty at end: yes
OK: single-node pop() verified under contention.

--- ASan (address + undefined) ---
Target:          100000
Total pushed:    100000
Total consumed:  100000
Stack empty at end: yes
OK: single-node pop() verified under contention.
```
5 consecutive TSan runs, all clean, all 100000/100000 — logged as five runs specifically because one clean pass proves nothing on lock-free code; both bugs above only surfaced after a real stress run, not on inspection.

C++ (`TrieberStack.cpp`, `make -f Makefile.cpp run-test`):
```
--- TSan (thread + undefined) ---
Target: 100000
Total nodes consumed: 100000

--- ASan (address + undefined) ---
Target: 100000
Total nodes consumed: 100000
```

## Notes / what broke
Two real bugs, both only found by actually running the sanitizers, neither visible from reading the code cold:
1. Tagged-pointer CAS alone: heap-use-after-free on the pre-CAS `head->next` read, caught by TSan on the first stress run of the naive tagged version.
2. First hazard-pointer fix: introduced a *new* data race by reusing `->next` for retire-list linkage, caught by TSan on the very next run after "fixing" bug 1. Fixed by giving retire-list linkage its own field.

The pattern across both: knowing the concept (ABA, reclamation, hazard pointers) and having correct code are different claims, and the gap between them only closed by forcing the code through TSan/ASan repeatedly, not by reasoning about it harder.

Original Makefile (before correction) referenced a source file (`TrieberStack.c`) that didn't match the actual filename (`treiber_stack.c`) and didn't link `-latomic` — meaning it had never actually been run after being written. Same discipline gap as Day 32's `atomics.cpp` scratch-file cleanup and the unwired `torn_detected` field: infrastructure and code both need to actually be executed before being called done, not just look complete.

**Logs.txt is not a retro.** Three lines — "easier than the queue," "wsl day," "wrap it up" — contain zero mention of ABA, zero mention of the two real bugs TSan caught, zero mention of the tagging-vs-reclamation distinction that was the actual point of the day. Day 32's own retro criteria (stated in that day's README) was explicit: if the log doesn't name a specific moment something almost broke, the stress test wasn't pushed hard enough. By that bar, today's log is a checkbox, not a reflection — the work was real and the bugs were real, but the written record of it doesn't show that. That's a gap between what got built and what got documented, and it's worth noticing that the gap exists on the exact day the deepest bugs of the whole concurrency arc showed up.

## Todo next
- Implement single-item `pop()` on the C++ side too (currently only bulk `steal()`), specifically to compare a *contended, unprotected* pop against the C version's tagged+hazard-protected one — right now the comparison is "two different designs," not "same design, different safety mechanism," which is the sharper version of this exercise.
- Retire-sweep is called on a fixed cadence (`local_count & 0xFF`) — untested whether that cadence causes unbounded retire-list growth under heavier producer/consumer imbalance than 10/10. Worth a stress run with skewed producer:consumer ratios.
- `MAX_HAZARD_THREADS 32` is a silent ceiling (`assert`-guarded, not handled) — fine for a 10-consumer test, a real landmine if this code is ever reused with more threads without noticing the constant.
- Write the actual retro tonight — name the specific TSan output line that mattered, not "learned about lock-free stacks."