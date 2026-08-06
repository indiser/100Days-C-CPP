# Day 22 — Ring Buffers `AUDIO`

## What
Build a fixed-capacity circular buffer for lock-free single-producer/single-consumer (SPSC) handoff — the pattern behind every real-time audio driver (ALSA, PortAudio, JUCE internals). C way: hand-rolled `LockFreeQueue` — flat `uint64_t` array, unmasked monotonically-increasing `head`/`tail` atomics, masked only at array-access time, each index cache-line-isolated via `alignas(64)` to kill false sharing. C++ way: `RingBuffer<T, Capacity>` — same layout, templated over element type and compile-time capacity, placement-`new`/explicit-destructor per slot for real RAII on non-trivial `T`, not just trivially-copyable types.

## Files
- `SPSC.c` — lock-free SPSC ring buffer, `uint64_t` payload, capacity 1024. Core ops: `initialize`, `queue_push`, `queue_pop`. Power-of-two capacity enforced via `_Static_assert`.
- `SPSC.cpp` — templated `RingBuffer<T, Capacity>` class. `try_emplace`/`try_push`/`try_pop`, placement-new construction, explicit destructor call on pop, `seed_indices()` exposed for wraparound testing.
- `AudioBuffer.c` / `AudioBuffer.cpp` — applied demo: same ring-buffer core repurposed as the actual boundary between a DSP producer thread (sine-wave generator, 440Hz) and a simulated real-time audio hardware callback pulling fixed-size frame blocks. Callback never blocks, never allocates — underrun path fills silence instead of stalling.
- `Logs.txt` — day log, includes sanitizer command history and the WSL/TSan fix.

## Build

C:
```
gcc -O2 -pthread -fsanitize=thread SPSC.c -o spsc_tsan && setarch $(uname -m) -R ./spsc_tsan
gcc -O2 -pthread -fsanitize=address,undefined SPSC.c -o spsc_asan && setarch $(uname -m) -R ./spsc_asan
```

C++:
```
g++ -O2 -std=c++20 -pthread -fsanitize=thread SPSC.cpp -o spsc_tsan && setarch $(uname -m) -R ./spsc_tsan
g++ -O2 -std=c++20 -pthread -fsanitize=address,undefined SPSC.cpp -o spsc_asan && setarch $(uname -m) -R ./spsc_asan
```

Same pattern for `AudioBuffer.c`/`.cpp`, `-lm` added for `sinf`.

## Design

**Unmasked monotonic indices, masked only on array access.** `head`/`tail` grow forever (as `size_t`), never wrap themselves — only `index & (Capacity - 1)` at the point of touching the array wraps. This sidesteps the classic full-vs-empty ambiguity entirely: `tail - head` gives exact element count via unsigned wraparound arithmetic, no sentinel slot sacrificed, no separate counter needed. Proven, not assumed — wraparound explicitly tested by seeding both indices to `SIZE_MAX - 1000` before the stress run, forcing the arithmetic to actually cross the overflow boundary mid-test.

**Memory ordering — relaxed load of own index, acquire load of other side's index, release store on publish.** Producer relaxed-loads its own `tail`, acquire-loads `head` to check space, writes data, then release-stores the new `tail`. Consumer mirrors it. Acquire/release pairing is what makes the data write visible-before-published to the other thread — anything looser (`relaxed` both sides) would let the consumer observe the new index before the data write lands, straight into a torn-read race.

**Cache-line isolation, not just correctness.** `head` and `tail` each pinned `alignas(64)` — without this, both indices share a cache line and every push/pop on either side invalidates the other core's cache line (false sharing), killing throughput even though the algorithm is still "correct." First C draft manually hand-padded with a byte array *before* `alignas(64)` — redundant, `alignas` already forces the padding, so the compiler pads it a second time. Removed, trusted `alignas` alone.

**SPSC only — no CAS needed.** Single producer means only one thread ever writes `tail`, single consumer means only one thread ever writes `head`. No compare-exchange loop required anywhere, unlike MPSC/MPMC which need CAS to arbitrate multiple writers on the same index. This is the actual reason SPSC is the easy case — worth being able to say out loud, not just have working code.

**C++ placement-new + explicit destructor, real RAII per slot.** Storage is raw uninitialized `std::aligned_storage_t<sizeof(T), alignof(T)>[Capacity]` — nothing constructed until pushed, explicitly destructed on pop. Makes the buffer usable for non-trivial `T` (strings, structs with destructors), not just POD types like the C version is stuck with.

**Real-time audio constraint modeled directly, not just described.** `audio_hardware_callback` never blocks, never allocates — on underrun (`ring_pop` returns false) it fills silence and moves on, exactly matching how a real driver callback must behave (missing a deadline there is an audible glitch, not just a slow frame). DSP producer thread is allowed to `usleep` when the buffer's full — sleeping on the producer side is fine, sleeping on the callback side is the one rule that can never break.

## Results
Both `SPSC.c`/`SPSC.cpp` — 100,000,000 push/pop ops, SPSC, wraparound seeded near `SIZE_MAX`:

```
C:   TSan clean, ASan/UBSan clean — "Passed 100000000 ops stress test + wraparound check"
C++: TSan clean, ASan/UBSan clean — "C++ RingBuffer passed 100000000 ops stress test + wraparound check"
```

`AudioBuffer.c`/`AudioBuffer.cpp` — 2 seconds simulated audio at 44.1kHz, 256-frame callback blocks:

```
C:   TSan clean (after ASLR fix), ASan/UBSan clean — "Audio callback processed 88320 frames without lock/alloc"
C++: TSan clean, ASan/UBSan clean — "C++ Audio callback processed 88320 frames without locks/allocs"
```

## Notes / what broke
`SPSC.c` first draft had no threading at all — `main()` did one push, one pop, single-threaded, called it done. Compiled clean, proved nothing about the actual race-prone case the day is about. Added real `pthread` producer/consumer with 100M-op hammering plus TSan/ASan before treating it as finished.

Hand-rolled pad array (56 bytes) placed before `alignas(64)` head field in first C draft — pointless, `alignas` already inserts equivalent padding on its own. Compiled fine, just redundant and confusing to read. Removed, kept `alignas` alone.

`assert(val == i)` used as the sole correctness oracle in both stress tests — silently compiles out under `-DNDEBUG`, meaning a release build would "pass" while checking nothing. Left in deliberately for stress-test-only code, documented here as a known trap rather than fixed, since this code never ships past testing.

ThreadSanitizer under WSL — `FATAL: ThreadSanitizer: unexpected memory mapping` on both `SPSC.cpp` and `AudioBuffer.c` — not a bug in the code, a known WSL/ASLR conflict with TSan's shadow-memory scheme. Fixed by disabling ASLR for the run: `setarch $(uname -m) -R ./binary`. Recorded here so it doesn't need rediscovering on a future TSan run.

`AudioBuffer.c`/`.cpp` demo never actually forces an underrun — producer (sine generator) comfortably keeps pace with the simulated 5ms hardware pull interval, so the buffer likely never empties when the callback reads it. The underrun-handling branch exists and is correct by inspection, but isn't exercised by this run. Not fixed — noted as a real gap, not a false claim of coverage.

`std::aligned_storage_t` is deprecated in C++23; build pinned to `-std=c++20` explicitly to avoid the warning/removal. Flagged for whenever the toolchain default moves — modern replacement is a raw `alignas(alignof(T)) std::byte[]` buffer.

## Todo next
- Force an actual underrun in the `AudioBuffer` demo (slow producer or speed up callback pull) and print/count when it happens — currently the safe path is untested in practice.
- Replace `assert`-based correctness check with an NDEBUG-safe manual check (`fprintf` + `exit`) if this code is ever reused past pure stress-testing.
- Migrate `std::aligned_storage_t` to `std::byte`-based raw storage ahead of any C++23 toolchain move.