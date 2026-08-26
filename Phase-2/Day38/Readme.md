# Day 38 — Producer-Consumer At Scale (Condition Variables / Bounded Buffer / Multi-Stage Pipeline)

## What
Real-time multi-stage audio pipeline in C and C++, three stages (producer → filter → consumer) connected by bounded queues, blocking on full/empty instead of dropping or spinning. C version: `pthread_mutex`+`pthread_cond_t`, manual ring buffer. C++ version: templated `BoundedPipelineQueue<T, Capacity>`, `std::mutex`+`std::condition_variable`, RAII lock via `std::unique_lock`. Both shut down cleanly via poison-pill frame, both stress-tested at 100,000 frames through a capacity-4 queue.

## Files
- `AudioPipeLine.c` — `Frame` struct (256-float sample chunk + `is_last` flag), `AudioQueue` bounded ring buffer (fixed array, `head`/`tail`/`count`, mutex + two cond vars), `queue_push`/`queue_pop` block on full/empty via predicate-loop `pthread_cond_wait`, three `pthread_t` stages (`stage1_producer` sine generator, `stage2_filter` gain stage, `stage3_consumer` drain+count), poison-pill frame propagated stage-to-stage for shutdown, `CHECK_PTHREAD` macro wraps every pthread call and exits loud on nonzero return
- `AudioPipeline.cpp` — `AudioFrame` struct (`std::array<float,256>` + `is_last`), `BoundedPipelineQueue<T, Capacity>` template (array-backed ring, `std::mutex`+2×`std::condition_variable`, `push`/`pop` take/return by move, predicate-lambda wait avoids spurious-wakeup bugs), same 3-stage pipeline as free functions run on `std::thread`, `std::ref` to pass queues, poison-pill `AudioFrame` shutdown same as C
- `fistTest.c` — standalone toy warm-up: two threads (`increment`/`decrement`) alternating via single shared `bool turn` + one cond var, predicate-loop wait, ping-pongs 10x each — minimal proof-of-concept for cond var mechanics before building the real pipeline

## Build
```
gcc -O2 -Wall -Wextra -g AudioPipeLine.c -o pipeline_c -lpthread -lm
g++ -std=c++20 -O2 -Wall -Wextra -g AudioPipeline.cpp -o pipeline_cpp -lpthread
```

## Run
```
./pipeline_c
./pipeline_cpp
```
C prints `Successfully consumed 100000 frames through pipeline.` + `Pipeline completed cleanly.` C++ prints `C++ Pipeline: consumed 100000 frames successfully.` + `Clean thread exit.`

## Design

### C — manual ring buffer, pthread primitives
- Bounded queue is a plain fixed-size array (`QUEUE_CAP = 4`) with `head`/`tail`/`count`, one `pthread_mutex_t`, two `pthread_cond_t` (`not_full`, `not_empty`) — classic two-condvar bounded-buffer pattern, no lock-free tricks, correctness over cleverness
- `queue_push`/`queue_pop` both use `while (cond) pthread_cond_wait(...)` — predicate re-checked after wakeup, immune to spurious wakeup and lost-wakeup class bugs
- Frame passed by pointer into `queue_push` (`const Frame *frame`) to cut one copy vs first draft; still one full 256-float copy into the ring slot and one back out — acceptable at this scale, flagged as future zero-copy target
- Poison pill (`is_last = true`) pushed once by producer, forwarded stage-to-stage unmodified by filter, consumer breaks loop on it — no shared atomic shutdown flag needed, shutdown is just another message in-band
- `CHECK_PTHREAD` macro added after first draft — every mutex/cond call checked, fails loud with `strerror` + file/line instead of silently corrupting state

### C++ — template queue, RAII locking
- `BoundedPipelineQueue<T, Capacity>` templated over payload type and capacity, `std::array` backing store, copies deleted (queue itself non-copyable) — same two-condvar shape as C, but `unique_lock`+lambda predicate (`not_full_.wait(ul, [this]{ return count_ < Capacity; })`) replaces manual `while`-loop, less code, same guarantee
- `push`/`pop` take/return by value with `std::move` — frame ownership transferred, no double-copy across queue boundary compared to C's copy-in/copy-out
- Stages are free functions run directly on `std::thread` with `std::ref(queue)` — no manual `pthread_t`/`void*` arg-struct boilerplate C needed
- Same poison-pill shutdown pattern as C, proves the mechanism is language-agnostic — the concept (in-band shutdown message through a blocking queue) doesn't change, only the plumbing does

## Correctness notes
- First cut of C version had zero return-value checking on any pthread call — silent failure on `mutex_init`/`cond_wait` would corrupt state with no trace. Fixed by wrapping every call in `CHECK_PTHREAD`
- First cut ran only 10 frames through a capacity-8 queue — never forced wraparound or real contention, so the ring-buffer index math was never actually exercised near its edges. Fixed: capacity dropped to 4, frame count raised to 100,000, forcing constant wrap and blocking on both sides
- Ran under ThreadSanitizer in WSL2 — hit `unexpected memory mapping` from tsan's shadow-memory scheme conflicting with WSL's ASLR layout, not a real bug in the code. Worked around with `setarch $(uname -m) -R ./pipeline` (disables ASLR for the run) rather than `-fsanitize=thread` failing outright
- tsan run under disabled ASLR completed silently with correct frame count (100,000) — no race reported. Silence + correct output treated as pass, but noted: haven't captured tsan's explicit "0 warnings" summary line, only inferred clean from absence of report + correct behavior
- C++ version has not yet been run under `-fsanitize=thread` in this environment — ran clean untested for races. Should not be trusted equally to the C side until it gets the same tsan treatment (with the same ASLR workaround if needed)
- Per-frame `printf` logging removed from the stress-test hot path (both C and C++) to make 100k-frame runs fast and readable — trades away per-frame visibility for throughput; acceptable for a stress pass, would need re-adding (or rate-limited) for real debugging

## Results
- C: 3-stage pipeline, capacity-4 bounded queue, 100,000 frames, clean poison-pill shutdown, no deadlock, no crash, correct count out — tsan-clean under ASLR workaround
- C++: same pipeline shape, templated queue, RAII locking, 100,000 frames, clean shutdown, correct count out — **not yet tsan-verified**, correctness currently resting on code inspection + one clean run only
- Should not be called "done" on clean-stdout alone, same lesson as Day 37 — one correct run proves nothing about race-freedom by itself. C side has tsan evidence behind it now; C++ side doesn't yet

## Todo next
- Move to Day 39: locks from scratch — spinlock via `atomic_flag`/CAS, RAII lock-guard wrapper, low-latency book access (`TRADE`)