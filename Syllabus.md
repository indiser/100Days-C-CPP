# 100 Days of C & C++ — Deep Systems + Applied Domains Roadmap
*Just like 100 Days of Python touched web dev, data science, automation, games, and APIs — this version makes sure C/C++ touch trading, graphics, audio, security, ML, blockchain, embedded, and gaming, not just abstract systems programming.*

**Format:** One concept per day. Build it in `C` and `C++`. Projects don't have to be identical — the underlying concept must match.

**Domain legend (tagged on every row):**
`SYS` systems/memory · `CONC` concurrency · `NET` networking/backend · `LANG` compilers/language internals · `TRADE` trading & quant finance · `GFX` graphics/rendering · `AUDIO` audio/DSP · `SEC` security/crypto · `ML` machine learning/numerical · `CHAIN` blockchain · `EMBED` embedded/robotics · `GAME` game dev · `DB` databases

**Your existing repos as reference:**
- [`indiser/DSA-Placement-Prep`](https://github.com/indiser/DSA-Placement-Prep) — algorithm reps, run in parallel, not part of this roadmap
- [`indiser/DSA-Projects`](https://github.com/indiser/DSA-Projects) — superseded in depth by this roadmap

---

## How To Use This Roadmap

```
100-days-c-cpp/
├── C/
│   ├── day01_.../
│   └── ...
├── CPP/
│   ├── day01_.../
│   └── ...
└── README.md
```

Every folder: source, `README.md` (what broke, what you learned), `Makefile`/`CMakeLists.txt` from Day 3 on. Commit daily.

### Tooling
`gcc`/`g++`, `clang`/`clang++` · `gdb`/`lldb`, `valgrind`, `-fsanitize=address,undefined,thread` · `perf`, `strace` · `cmake`, `ninja` · `clang-tidy`, `cppcheck` · godbolt.org for codegen, use it daily.

---

## Phase 0 — Toolchain & Modern Language Gap-Fill (Days 1–5) `SYS`

| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 1 | Optimization levels & codegen | Benchmark a string routine across `-O0`–`-O3`, read assembly diff | Same, templated version vs non-templated codegen |
| 2 | Modern generics | Type-safe generic macro lib via `_Generic` | Concept-constrained generic lib (C++20 `concepts`) |
| 3 | Build systems | CMake multi-target: static+shared lib+exe | Same, C++20 modules experiment |
| 4 | Profiling | Fix a real perf bug in matrix-multiply via `perf` | Same routine, templated, assembly comparison |
| 5 | Fuzzing | libFuzzer/AFL harness for a hand-written parser | Property-based test harness for same parser |

---

## Phase 1 — Memory & Systems Depth, Domain-Flavored (Days 6–30) `SYS`

| Day | Concept | Domain | C Project | C++ Project |
|---|---|---|---|---|
| 6 | Arena allocators | `GAME` | Arena allocator for game asset loading | Same, as pluggable C++ allocator |
| 7 | Alignment & padding | `TRADE` | Cache-line-aligned market-tick struct | Same via `alignas`/`alignof` |
| 8 | `mmap` | `DB` | Memory-mapped log-file grep tool | Same, RAII-wrapped mapping |
| 9 | Buddy allocator | `EMBED` | Buddy allocator for a constrained-memory device sim | Same, wrapped as C++ allocator |
| 10 | Cache-friendly layout | `GFX` | Particle system: AoS vs SoA benchmark | Same, templated over layout policy |
| 11 | Lock-free basics | `TRADE` | CAS-based lock-free order queue | Same using `std::atomic` |
| 12 | Smart pointer internals | `SYS` | Manual refcounted pointer (macros) | Your own `unique_ptr`/`shared_ptr` |
| 13 | Binary serialization | `TRADE` | FIX-like market-data serializer | Template-based serialization lib |
| 14 | Endianness & binary formats | `GFX` | BMP image parser, byte-exact | Same, class-based |
| 15 | Signal handling | `SYS` | `SIGSEGV` handler with backtrace | Exception-safe crash handler |
| 16 | String interning | `LANG` | Interning pool for a symbol table | COW string class from scratch |
| 17 | Leak detection | `SEC` | Wrap `malloc`/`free` to build a leak tracker | Override `new`/`delete` to track allocations |
| 18 | Placement construction | `GAME` | Manual object construction in raw buffer | Object pool using placement `new` |
| 19 | Tagged unions | `GAME` | Tagged-union item/inventory variant system | `std::variant` + visitor for same |
| 20 | Bit-packed structures | `GAME` | Bit-packed chess board representation | Same, `constexpr` bit tricks |
| 21 | Small string optimization | `LANG` | Dynamic string with SSO, from scratch | Mini `std::string` clone with SSO |
| 22 | Ring buffers | `AUDIO` | Lock-free single-producer audio ring buffer | Templated ring buffer class |
| 23 | Object pools | `GAME` | Game-entity object pool | Templated pool allocator |
| 24 | High-perf hash maps | `TRADE` | Cache-aware symbol lookup table | Same, benchmarked vs `std::unordered_map` |
| 25 | Paged storage | `DB` | Simple paged storage engine (SQLite-pager-style) | Same, class-based page cache |
| 26 | Zero-copy parsing | `TRADE` | Zero-copy market-data feed parser | Same using `std::string_view` |
| 27 | Custom STL allocators | `GAME` | Generic allocator interface (C vtable-style) | Custom allocator plugged into `std::vector` |
| 28 | Undefined behavior | `SEC` | Build + detect UB catalog (this is where real vulns come from) with UBSan | Same, note C vs C++ UB divergence |
| 29 | Benchmark harness | `TRADE` | Latency micro-benchmark framework | Same, templated over callable type |
| 30 | **Checkpoint** | `TRADE` | Pointer-based LRU price cache | Templated LRU cache, benchmarked vs Day-30 C version |

---

## Phase 2 — Concurrency, Networking, OS, Domain-Flavored (Days 31–55) `CONC` `NET`

| Day | Concept | Domain | C Project | C++ Project |
|---|---|---|---|---|
| 31 | Thread lifecycle | `TRADE` | `pthread`-based market-data ingest worker pool | `std::thread`-based worker pool |
| 32 | Atomics & memory ordering | `TRADE` | Atomic price ticker, explore `memory_order` | Same via `std::atomic`, relaxed vs seq_cst benchmark |
| 33 | Lock-free queues | `TRADE` | MPSC lock-free order queue | Same |
| 34 | Real thread pools | `GAME` | Job-system thread pool | Thread pool returning `std::future` |
| 35 | Async I/O (`epoll`) | `TRADE` | Event-loop market-data feed handler | Same, wrapped as `Reactor` class |
| 36 | Coroutines | `GAME` | Coroutines via `ucontext` for NPC scripting | C++20 coroutine generator, same use case |
| 37 | Actor model | `TRADE` | Message-passing actor system (order routing) | Same, class-based actors |
| 38 | Producer-consumer at scale | `AUDIO` | Multi-stage real-time audio processing pipeline | Same using `condition_variable` |
| 39 | Locks from scratch | `TRADE` | Spinlock for low-latency order book access | Same, RAII `lock_guard`-style wrapper |
| 40 | Deadlock detection | `SYS` | Simulate + detect deadlock via resource graph | Same |
| 41 | Raw sockets, custom protocol | `TRADE` | Custom binary market-data protocol server | Same, class-based protocol handler |
| 42 | HTTP parsing | `NET` | HTTP/1.1 request parser from scratch | Same |
| 43 | Concurrent HTTP server | `NET` | Multithreaded HTTP server | Same, OOP router + middleware |
| 44 | WebSocket protocol | `TRADE` | Live price-streaming WebSocket server | Same |
| 45 | DNS from scratch | `NET` | Mini DNS client, raw UDP | Same |
| 46 | RPC framework | `NET` | RPC over TCP, manual serialization | Same, template-based dispatch |
| 47 | Unix domain sockets | `SYS` | IPC via Unix sockets | Same |
| 48 | Shared memory + semaphores | `TRADE` | Shared-memory IPC between trading processes (real HFT pattern) | Same, RAII-wrapped |
| 49 | Process management | `SYS` | Mini init/supervisor system | Same |
| 50 | Filesystem internals | `SYS` | Minimal FUSE filesystem | Same |
| 51 | Syscall tracing | `SEC` | Mini `strace` clone via `ptrace` | Same |
| 52 | Container internals | `SYS` | Mini container runtime (namespaces+chroot) | Same |
| 53 | Async logging | `TRADE` | Lock-free async trade-audit logger | Same, structured logging via templates |
| 54 | Load balancing | `NET` | TCP round-robin load balancer | Same |
| 55 | **Checkpoint** | `NET`/`TRADE` | Async multithreaded market-data proxy | Same system, full C++ build, benchmark both |

---

## Phase 3 — Language Internals + Applied Domain Sprints (Days 56–90)

### 3A. Compilers & VMs (Days 56–65) `LANG`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 56 | Lexing | Tokenizer for a language subset | Same, `string_view`-based tokens |
| 57 | Recursive descent parsing | AST via tagged-union structs | AST via smart-pointer node hierarchy |
| 58 | Tree-walking interpreter | Interpreter: arithmetic+vars+functions | Same, visitor-pattern evaluation |
| 59 | Bytecode compilation | Compile AST → bytecode | Same |
| 60 | Stack-based VM | Full VM: calls, locals, closures | Same |
| 61 | Garbage collection | Mark-sweep GC for toy VM | Same |
| 62 | JIT basics | Generate + execute raw machine code for expr eval | Same |
| 63 | Type erasure | Manual fn-pointer + `void*` context | Implement `std::function` from scratch |
| 64 | Template metaprogramming | *(study C's `_Generic`/macro equivalent instead)* | Compile-time type-list / prime-checker library |
| 65 | CRTP & static polymorphism | *(compare to C's fn-pointer vtables)* | Plugin system via CRTP |

### 3B. Graphics & Rendering (Days 66–69) `GFX`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 66 | Software rasterization | Line + triangle rasterizer, framebuffer to PPM | Same, class-based `Canvas` |
| 67 | 3D math pipeline | Vector/matrix lib + perspective projection | Same, operator-overloaded `Vec3`/`Mat4` |
| 68 | Ray tracing basics | Sphere + point-light ray tracer, output PPM | Same, OOP `Hittable` hierarchy |
| 69 | Image processing | Convolution filters: blur, edge-detect, sharpen | Same, templated over kernel |

### 3C. Audio & DSP (Days 70–72) `AUDIO`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 70 | Signal generation | Sine/square wave synth → WAV file output | Same, class-based oscillator |
| 71 | Fourier transform | FFT implementation from scratch | Same, templated over sample type |
| 72 | Real-time effects | Delay/echo audio effect processor | Same, chainable effect pipeline |

### 3D. Security & Cryptography (Days 73–76) `SEC`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 73 | Hash functions | SHA-256 implementation from spec | Same, `constexpr`-friendly where possible |
| 74 | Symmetric ciphers | AES (or reduced-round) implementation from scratch | Same, class-based cipher context |
| 75 | Handshake protocols | Simulated TLS-lite handshake (key exchange sim) | Same, OOP protocol state machine |
| 76 | Sandboxing | `seccomp`-based syscall filter/sandbox | Same, RAII sandbox guard |

### 3E. Machine Learning & Numerical (Days 77–80) `ML`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 77 | Linear algebra | Matrix/vector library, from scratch | Same, templated + operator overloading |
| 78 | Gradient descent | Linear regression trained by hand | Same |
| 79 | Neural networks | MLP + backprop from scratch, no libs | Same, class-based `Layer`/`Network` |
| 80 | Classical ML | Decision tree or naive Bayes classifier | Same |

### 3F. Trading & Quant Finance (Days 81–85) `TRADE`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 81 | Order book | Limit order book: add/cancel/modify | Same, class-based, benchmark ops/sec |
| 82 | Matching engine | Price-time priority matching engine | Same |
| 83 | Market data normalization | Feed handler parsing + normalizing raw ticks | Same |
| 84 | Backtesting | Backtest engine replaying historical ticks vs a strategy | Same |
| 85 | Options pricing | Black-Scholes + Monte Carlo pricer | Same |

### 3G. Blockchain (Days 86–87) `CHAIN`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 86 | Merkle trees & blocks | Merkle tree + block structure w/ hash linking | Same |
| 87 | Proof of work | PoW miner + chain validation | Same |

### 3H. Embedded & Robotics (Days 88–90) `EMBED`
| Day | Concept | C Project | C++ Project |
|---|---|---|---|
| 88 | Bare-metal patterns | GPIO/blink simulator + RTOS-lite cooperative scheduler | Same |
| 89 | Control systems | PID controller simulation | Same |
| 90 | Sensor fusion | Simple Kalman filter on noisy sensor data | Same |

---

## Phase 4 — Capstones (Days 91–100)

**Capstone A (91–95): Low-Latency Trading System**
Wire together Days 81–85 + Phase 2's networking/concurrency work: order book + matching engine + market-data feed handler + risk checks + backtester. Benchmark tick-to-trade latency. Build both a full C and full C++ version, compare latency numbers directly — this comparison *is* the deliverable.

**Capstone B (96–100): pick one track**
- **Game track** `GAME`/`GFX` — ECS architecture + physics + your Day 66–68 rasterizer/ray tracer as the renderer, ship a small playable game
- **Security track** `SEC` — mini network scanner + intrusion-detection tool using Day 73–76 crypto work
- **ML track** `ML` — small ML-powered tool (e.g. anomaly detector) built on Day 77–80 work, applied to Day 83's market-data stream

Both capstones: full `README.md`, architecture diagram, tests, CI, and — for A specifically — a written latency comparison between the C and C++ builds.

---

## Daily Ritual
1. Build the C version — feel where the language gives you nothing.
2. Build the C++ version — see what the abstraction buys you, and what it costs.
3. Write 3–5 lines: what broke, what surprised you, which version you'd actually ship.
4. Commit both.

## Reference Material
- *C Programming: A Modern Approach* — K.N. King
- *Effective Modern C++* — Scott Meyers
- *C++ Templates: The Complete Guide* — Vandevoorde/Josuttis/Gregor
- *The Linux Programming Interface* — Michael Kerrisk (Phase 2)
- *Crafting Interpreters* — Robert Nystrom (Phase 3A)
- *Physically Based Rendering* (skim relevant chapters) — Phase 3B
- *Trading and Exchanges* — Larry Harris (Phase 3F, concepts not code)
- *Mastering Bitcoin* (skim protocol chapters) — Phase 3G
- godbolt.org — daily, not optional
