# 100 Days of C & C++
### A Deep Systems Roadmap — Memory, Concurrency, Compilers, Trading, Graphics, and More

> *Build everything twice. Once in C to feel the machine. Once in C++ to see what abstraction costs.*

---

## What This Is

A structured 100-day curriculum covering low-level systems programming in both C and C++ — not toy programs, not tutorials. Every day targets a real concept, builds a real artifact, and documents what broke and why. Domains span trading systems, graphics, audio DSP, security, machine learning, blockchain, embedded, and game dev.

The rule: study the concept cold first. Then build in C. Then build in C++. Then write down what broke.

---

## Progress

| Phase | Days | Theme | Status |
|---|---|---|---|
| **Phase 0** | 1–5 | Toolchain & Modern Gap-Fill | ✅ Complete |
| **Phase 1** | 6–30 | Memory & Systems, Domain-Flavored | 🔄 In Progress (Day 17) |
| **Phase 2** | 31–55 | Concurrency, Networking, OS | ⏳ Upcoming |
| **Phase 3** | 56–90 | Language Internals + Applied Domain Sprints | ⏳ Upcoming |
| **Phase 4** | 91–100 | Capstones | ⏳ Upcoming |

---

## Phase 0 — Toolchain & Modern Gap-Fill `Days 1–5`

Getting the environment right and filling modern C/C++ gaps before touching systems code.

| Day | Concept | Highlight |
|---|---|---|
| [Day 1](Phase-0/Day1/Readme.md) | Optimization & Codegen | Benchmarked string-reverse across `-O0`–`-O3`. Proved 3.3× cliff at O0→O1, near-zero gain O2→O3. Read the asm diff by hand. |
| [Day 2](Phase-0/Day2/Readme.md) | Modern Generics | `_Generic` dispatch in C11 vs C++20 `concepts`/`requires`. Proved `NDEBUG` silently kills `assert`, `static_assert` is immune. |
| [Day 3](Phase-0/Day3/Readme.md) | Build Systems | CMake multi-target: static lib, shared lib, two executables. Proved shared-lib deploy risk by deleting the `.dll` and watching it crash. |
| [Day 4](Phase-0/Day4/Readme.md) | Profiling | Naive → loop-reordered → tiled matrix multiply. ~10× speedup from one loop-order swap, further ~30% from tiling. `perf` hardware counters unreliable in this env — wall-clock told the real story. |
| [Day 5](Phase-0/Day5/Readme.md) | Fuzzing | libFuzzer harness on a hand-rolled TLV parser. C and C++ versions, ASan/UBSan wired. 33M+ execs, bounds check held. Bug-trap variant (intentional overflow) still open. |

---

## Phase 1 — Memory & Systems, Domain-Flavored `Days 6–30`

Every day targets a memory or systems concept, applied to a real domain — trading, graphics, embedded, game dev, security.

| Day | Concept | Domain | Highlight |
|---|---|---|---|
| [Day 6](Phase-1/Day6/Readme.md) | Arena Allocators | `GAME` | Bump-pointer arena backed by `VirtualAlloc` (Windows). C++ version exposed as a pluggable `Allocator` for `std::vector`. O(1) bulk reset, no per-object free. |
| [Day 7](Phase-1/Day7/Readme.md) | Alignment & Padding | `TRADE` | Cache-aligned `MarketTick` via `alignas(64)`. Concurrent false-sharing benchmark — proved the cost in wall-clock time, not just theory. |
| [Day 8](Phase-1/Day8/Readme.md) | `mmap` | `DB` | Memory-mapped grep vs `fread`-based grep. Win32 `CreateFileMapping`/`MapViewOfFile`, RAII-wrapped C++ version. Prefetch hint via `PrefetchVirtualMemory`. |
| [Day 9](Phase-1/Day9/Readme.md) | Buddy Allocator | `EMBED` | Power-of-two splitting, XOR buddy address, free-list coalescing. 5000-round stress test: no overlaps, full coalesce back to single root block verified by assertion. |
| [Day 10](Phase-1/Day10/Readme.md) | Cache-Friendly Layout | `GFX` | AoS vs SoA particle system. SoA 1.2–2.5× faster in timing. Cache-miss counters contradicted theory — WSL `perf` unreliable, honest about it. |
| [Day 11](Phase-1/Day11/Readme.md) | Lock-Free Basics | `TRADE` | SPSC + MPMC bounded queues. Vyukov sequence-per-slot design. 4 producers × 100k items, verified under ThreadSanitizer: 0 drops, 0 dupes, 0 corruption. Hardest day so far. |
| [Day 12](Phase-1/Day12/Readme.md) | Smart Pointer Internals | `SYS` | Manual refcounted pointer in C. Hand-rolled `unique_ptr` (move-only, deleted copy) and `shared_ptr` (control block, copy bumps refcount) in C++. |
| [Day 13](Phase-1/Day13/Readme.md) | Binary Serialization | `TRADE` | FIX-like market-data serializer. Manual big-endian encode/decode in C. C++ version adds `write_be<T>`/`read_be<T>` template dispatch via `if constexpr`. |
| [Day 14](Phase-1/Day14/Readme.md) | Endianness / Binary Formats | `GFX` | Byte-exact BMP parser+writer. `#pragma pack(1)` struct overlay, row-padding handled, color inversion. C++ version wrapped in `BMPImage` RAII class. |
| [Day 15](Phase-1/Day15/Readme.md) | Signal Handling | `SYS` | `SIGSEGV` handler with `backtrace_symbols_fd`. Stack-overflow detection via `sigaltstack` + `si_addr` range check. Async-signal-safe throughout (`write()`, not `printf`). |
| [Day 16](Phase-1/Day16/Readme.md) | String Interning & COW | `LANG` | Hash-based interning pool (open addressing, FNV-1a) for O(1) equality. Reference-counted copy-on-write string in C++ — copy is O(1), write triggers `detach_if_shared()`. |
| [Day 17](Phase-1/Day17/Readme.md) | Memory Interception & Leak Tracking | `MEM` | Three hook strategies: LD_PRELOAD + `dlsym(RTLD_NEXT)`, macro redirection, global `operator new/delete` overload. Reentrancy guard is the real boss fight. Leak detection confirmed on real leaks. |

*Days 18–30 upcoming: placement new, tagged unions, bit-packing, SSO strings, ring buffers, object pools, high-perf hash maps, paged storage, zero-copy parsing, custom STL allocators, UB catalog, benchmark harness, LRU cache checkpoint.*

---

## Phase 2 — Concurrency, Networking, OS `Days 31–55`

Thread lifecycle, atomics, lock-free queues, `epoll`, coroutines, raw sockets, HTTP, WebSocket, DNS, RPC, shared memory, containers, async logging.

---

## Phase 3 — Language Internals + Applied Domain Sprints `Days 56–90`

**3A Compilers & VMs** — Lexer → parser → AST → bytecode → stack VM → GC → JIT basics  
**3B Graphics** — Rasterizer, 3D math pipeline, ray tracer, image processing  
**3C Audio & DSP** — Sine/square wave synth, FFT from scratch, real-time effects  
**3D Security & Crypto** — SHA-256, AES, Diffie-Hellman, `seccomp-bpf` sandboxing  
**3E Machine Learning** — Linear algebra lib, gradient descent, MLP + backprop, decision tree  
**3F Trading & Quant** — Order book, matching engine, feed handler, backtester, Black-Scholes  
**3G Blockchain** — Merkle trees, proof-of-work miner  
**3H Embedded & Robotics** — Bare-metal patterns, PID controller, Kalman filter  

---

## Phase 4 — Capstones `Days 91–100`

**Capstone A (91–95):** Full low-latency trading system — order book + matching engine + feed handler + risk checks + backtester. C and C++ versions, latency benchmarked head-to-head.

**Capstone B (96–100):** One of:
- Game engine with ECS, physics, and renderer
- Security toolkit with network scanning and intrusion detection
- ML anomaly detection on streaming market data

---

## Toolchain

```
Compilers   gcc/g++, clang/clang++
Debuggers   gdb, lldb
Sanitizers  -fsanitize=address,undefined,thread
Profiling   perf, valgrind, strace
Build       CMake, Ninja
Analysis    clang-tidy, cppcheck, godbolt.org
```

---

## Reference Library

| Book | Used In |
|---|---|
| *C Programming: A Modern Approach* — K.N. King | Phase 0–1 |
| *Effective Modern C++* — Scott Meyers | Phase 0–2 |
| *C++ Templates: The Complete Guide* — Vandevoorde/Josuttis/Gregor | Phase 1–3 |
| *The Linux Programming Interface* — Michael Kerrisk | Phase 2 |
| *Crafting Interpreters* — Robert Nystrom | Phase 3A |
| *Physically Based Rendering* | Phase 3B |
| *Trading and Exchanges* — Larry Harris | Phase 3F |
| *Mastering Bitcoin* | Phase 3G |
| *Building Low Latency Applications with C++* — Sourav Ghosh | Phase 4 |
| *C++ Concurrency in Action* — Anthony Williams | Phase 2–4 |

All books are in [`/Books`](Books/).

---

## Daily Ritual

1. Study the concept cold — no coding until the theory is clear
2. Build the C version — feel where the language gives you nothing
3. Build the C++ version — see what abstraction buys and what it costs
4. Write down what broke and what you'd actually ship
5. Commit both

---

## Parallel Tracks

- [`indiser/DSA-Placement-Prep`](https://github.com/indiser/DSA-Placement-Prep) — algorithm reps, running in parallel
