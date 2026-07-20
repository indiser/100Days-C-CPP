# 100 Days of C & C++ — Deep Systems + Applied Domains Roadmap
*Touches trading, graphics, audio, security, ML, blockchain, embedded, gaming — not just abstract systems. Every day now lists exact topics to study before building.*

**Format:** One concept per day. Build in `C` and `C++`. Projects don't need to match exactly — concept must.

**Domain legend:** `SYS` systems/memory · `CONC` concurrency · `NET` networking · `LANG` compilers · `TRADE` trading/finance · `GFX` graphics · `AUDIO` audio/DSP · `SEC` security/crypto · `ML` machine learning · `CHAIN` blockchain · `EMBED` embedded/robotics · `GAME` game dev · `DB` databases

**Repos:** [`indiser/DSA-Placement-Prep`](https://github.com/indiser/DSA-Placement-Prep) — algo reps, parallel track. [`indiser/DSA-Projects`](https://github.com/indiser/DSA-Projects) — superseded by this.

```
100-days-c-cpp/
├── C/day01_.../ ...
├── CPP/day01_.../ ...
└── README.md
```
Tooling: gcc/g++, clang/clang++, gdb/lldb, valgrind, `-fsanitize=address,undefined,thread`, perf, strace, cmake, ninja, clang-tidy, cppcheck, godbolt.org.

---

## Phase 0 — Toolchain & Modern Gap-Fill (Days 1–5) `SYS`

| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 1 | Optimization/codegen | gcc/g++ `-O0`–`-O3` flags, compilation stages, `objdump`, `clock_gettime`/`chrono` timing | Benchmark string routine across opt levels, read asm diff | Same, templated vs non-templated codegen |
| 2 | Modern generics | `_Generic` (C11), `static_assert`, C++20 `concepts`/`requires` | Type-safe generic macro lib via `_Generic` | Concept-constrained generic lib |
| 3 | Build systems | CMake (`add_library`, `add_executable`, `target_link_libraries`), static vs shared libs, C++20 modules | Multi-target CMake: static+shared+exe | Same + modules experiment |
| 4 | Profiling | `perf stat/record/report`, cache misses, loop blocking/tiling | Fix perf bug in matrix-multiply | Same, templated, asm comparison |
| 5 | Fuzzing | libFuzzer/AFL harness basics, property-based testing concepts | Fuzz harness for hand-written parser | Property-based test harness, same parser |

---

## Phase 1 — Memory & Systems, Domain-Flavored (Days 6–30) `SYS`

| Day | Concept | Domain | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|---|
| 6 | Arena allocators | `GAME` | `mmap`/`sbrk`, pointer-bump allocation, C++ Allocator interface | Arena allocator for game asset loading | Same as pluggable C++ allocator |
| 7 | Alignment & padding | `TRADE` | `alignas`/`alignof`, struct padding rules, cache-line size, false sharing | Cache-aligned market-tick struct | Same via `alignas` |
| 8 | `mmap` | `DB` | `mmap`/`munmap`, page size, file-backed mapping, `madvise` | Memory-mapped log grep tool | Same, RAII-wrapped |
| 9 | Buddy allocator | `EMBED` | Buddy allocation algorithm, power-of-two splitting, free lists | Buddy allocator for constrained-memory sim | Same, wrapped as C++ allocator |
| 10 | Cache-friendly layout | `GFX` | Data layout (AoS/SoA), cache locality, benchmarking | Particle system AoS vs SoA benchmark | Same, templated over layout |
| 11 | Lock-free basics | `TRADE` | CAS/`compare_exchange`, ABA problem, memory ordering basics | Lock-free order queue | Same via `std::atomic` |
| 12 | Smart pointer internals | `SYS` | Reference counting, move semantics, RAII, operator overload (`*`,`->`) | Manual refcounted pointer | Your own `unique_ptr`/`shared_ptr` |
| 13 | Binary serialization | `TRADE` | Struct packing, endianness, manual byte buffers, C++ templates | FIX-like market-data serializer | Template-based serialization lib |
| 14 | Endianness/binary formats | `GFX` | Big/little endian, `htons`/`ntohl`, BMP format spec | BMP parser, byte-exact | Same, class-based |
| 15 | Signal handling | `SYS` | `signal`/`sigaction`, `SIGSEGV`, `backtrace()`, async-signal-safety | `SIGSEGV` handler w/ backtrace | Exception-safe crash handler |
| 16 | String interning | `LANG` | Hash tables, string hashing, symbol table design, COW concept | Interning pool for symbol table | COW string class from scratch |
| 17 | Leak detection | `SEC` | `malloc`/`free` hooking, `operator new`/`delete` overriding | Wrap malloc/free to track allocations | Override new/delete to track allocations |
| 18 | Placement construction | `GAME` | Placement `new`, manual destructor calls, object lifetime rules | Manual object construction in raw buffer | Object pool using placement `new` |
| 19 | Tagged unions | `GAME` | C `union`+enum tag dispatch, `std::variant`/`std::visit` | Tagged-union item/inventory system | `std::variant`+visitor for same |
| 20 | Bit-packed structures | `GAME` | Bitfields, bit shift/mask, `constexpr` | Bit-packed chess board | Same, `constexpr` bit tricks |
| 21 | Small string optimization | `LANG` | Union-based storage, SBO, capacity/growth strategy | Dynamic string w/ SSO from scratch | Mini `std::string` clone w/ SSO |
| 22 | Ring buffers | `AUDIO` | Circular indexing/modulo, lock-free SPSC pattern | Lock-free audio ring buffer | Templated ring buffer class |
| 23 | Object pools | `GAME` | Free-list management, placement new, pooling pattern | Game-entity object pool | Templated pool allocator |
| 24 | High-perf hash maps | `TRADE` | Open addressing, probing strategies, hash design, load factor | Cache-aware symbol lookup table | Same, benchmarked vs `unordered_map` |
| 25 | Paged storage | `DB` | `pread`/`pwrite`, page-based storage design, LRU page cache | Paged storage engine (SQLite-pager-style) | Same, class-based page cache |
| 26 | Zero-copy parsing | `TRADE` | Pointer arithmetic, `std::string_view`, allocation-free parsing | Zero-copy market-data feed parser | Same using `string_view` |
| 27 | Custom STL allocators | `GAME` | C++ Allocator named requirements, `rebind`, `propagate_on_*` | Generic allocator interface (C vtable-style) | Custom allocator plugged into `vector` |
| 28 | Undefined behavior | `SEC` | UBSan flags, signed overflow, strict aliasing, uninit reads | Build+detect UB catalog w/ UBSan | Same, note C vs C++ UB divergence |
| 29 | Benchmark harness | `TRADE` | `chrono::steady_clock`, mean/stddev measurement, warm-up | Latency micro-benchmark framework | Same, templated over callable |
| 30 | **Checkpoint** | `TRADE` | Hashmap+doubly-linked-list combo, eviction policy | Pointer-based LRU price cache | Templated LRU cache, benchmark vs C |

---

## Phase 2 — Concurrency, Networking, OS (Days 31–55) `CONC` `NET`

| Day | Concept | Domain | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|---|
| 31 | Thread lifecycle | `TRADE` | `pthread_create/join/detach`, `std::thread` | Market-data ingest worker pool | `std::thread`-based worker pool |
| 32 | Atomics & ordering | `TRADE` | `stdatomic.h`, `memory_order_relaxed/acquire/release/seq_cst` | Atomic price ticker | Same via `std::atomic`, perf compare |
| 33 | Lock-free queues | `TRADE` | MPSC queue design, CAS loops, memory reclamation basics | MPSC lock-free order queue | Same |
| 34 | Real thread pools | `GAME` | Task queue, `condition_variable`, `future`/`promise` | Job-system thread pool | Thread pool returning `std::future` |
| 35 | Async I/O (`epoll`) | `TRADE` | `epoll_create/ctl/wait`, non-blocking sockets | Event-loop market-data feed handler | Same, wrapped as `Reactor` class |
| 36 | Coroutines | `GAME` | `ucontext.h` (`makecontext`/`swapcontext`), C++20 `co_await`/`co_yield` | Coroutines via ucontext for NPC scripting | C++20 coroutine generator, same use |
| 37 | Actor model | `TRADE` | Message queues, mailbox pattern, async dispatch | Actor system for order routing | Same, class-based actors |
| 38 | Producer-consumer at scale | `AUDIO` | `condition_variable`, mutex, bounded buffer | Multi-stage real-time audio pipeline | Same via `condition_variable` |
| 39 | Locks from scratch | `TRADE` | Spinlock via `atomic_flag`/CAS, busy-wait, RAII guard | Spinlock for low-latency book access | Same, RAII lock-guard wrapper |
| 40 | Deadlock detection | `SYS` | Resource allocation graph, cycle detection, lock ordering | Simulate + detect deadlock | Same |
| 41 | Raw sockets, custom protocol | `TRADE` | `socket/bind/send/recv`, custom binary protocol design | Custom binary market-data protocol server | Same, class-based handler |
| 42 | HTTP parsing | `NET` | HTTP/1.1 spec, state-machine parsing, header parsing | HTTP/1.1 request parser | Same |
| 43 | Concurrent HTTP server | `NET` | Thread-per-conn or pool serving, routing table design | Multithreaded HTTP server | Same, OOP router+middleware |
| 44 | WebSocket protocol | `TRADE` | RFC 6455 handshake, frame format, masking | Live price-streaming WebSocket server | Same |
| 45 | DNS from scratch | `NET` | UDP sockets, DNS packet format, resource records | Mini DNS client, raw UDP | Same |
| 46 | RPC framework | `NET` | TCP framing, request/response correlation IDs | RPC over TCP, manual serialization | Same, template-based dispatch |
| 47 | Unix domain sockets | `SYS` | `AF_UNIX` sockets, local IPC | IPC via Unix sockets | Same |
| 48 | Shared memory+semaphores | `TRADE` | `shm_open`/`mmap`, `sem_open/wait/post` | Shared-memory IPC between trading procs | Same, RAII-wrapped |
| 49 | Process management | `SYS` | `fork/exec/wait/waitpid`, signal forwarding | Mini init/supervisor system | Same |
| 50 | Filesystem internals | `SYS` | FUSE API (`fuse_operations`), inode/VFS concepts | Minimal FUSE filesystem | Same |
| 51 | Syscall tracing | `SEC` | `ptrace` (`PTRACE_TRACEME`/`PEEKTEXT`), syscall interception | Mini `strace` clone | Same |
| 52 | Container internals | `SYS` | `unshare`/`clone` w/ namespaces, `chroot`, cgroups basics | Mini container runtime | Same |
| 53 | Async logging | `TRADE` | Lock-free queue reuse, background writer thread | Async trade-audit logger | Same, structured via templates |
| 54 | Load balancing | `NET` | Round-robin algorithm, connection forwarding, health checks | TCP round-robin load balancer | Same |
| 55 | **Checkpoint** | `NET`/`TRADE` | Combine epoll+thread pool+protocol parsing | Async multithreaded market-data proxy | Same, full C++ build, benchmark both |

---

## Phase 3 — Language Internals + Applied Domain Sprints (Days 56–90)

### 3A. Compilers & VMs (56–65) `LANG`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 56 | Lexing | Finite automata basics, tokenization | Tokenizer for a language subset | Same, `string_view`-based tokens |
| 57 | Recursive descent parsing | Grammar rules (EBNF), AST design, operator precedence | AST via tagged-union structs | AST via smart-pointer node hierarchy |
| 58 | Tree-walking interpreter | Visitor pattern, scope/environment management | Interpreter: arithmetic+vars+functions | Same, visitor-pattern eval |
| 59 | Bytecode compilation | Instruction set design, AST-to-bytecode lowering | Compile AST → bytecode | Same |
| 60 | Stack-based VM | Stack machine loop, call frames, opcode dispatch | Full VM: calls, locals, closures | Same |
| 61 | Garbage collection | Mark-sweep algorithm, root-set tracing | Mark-sweep GC for toy VM | Same |
| 62 | JIT basics | `mmap` w/ `PROT_EXEC`, machine code gen, calling conventions | Generate+execute raw machine code | Same |
| 63 | Type erasure | Fn-pointer+`void*` context, vtable-like dispatch | Manual fn-pointer+void* context | Implement `std::function` from scratch |
| 64 | Template metaprogramming | Template specialization, SFINAE, type traits, variadics | *(study C's `_Generic` equivalent)* | Compile-time type-list/prime-checker |
| 65 | CRTP | Curiously Recurring Template Pattern, static polymorphism | *(compare to C fn-pointer vtables)* | Plugin system via CRTP |

### 3B. Graphics & Rendering (66–69) `GFX`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 66 | Rasterization | Bresenham line algorithm, barycentric coords, PPM format | Line+triangle rasterizer to PPM | Same, class-based `Canvas` |
| 67 | 3D math pipeline | Vector/matrix math, homogeneous coords, perspective projection | Vec/mat lib + perspective projection | Same, operator-overloaded `Vec3`/`Mat4` |
| 68 | Ray tracing basics | Ray-sphere intersection, Phong/Lambertian lighting | Sphere+point-light ray tracer | Same, OOP `Hittable` hierarchy |
| 69 | Image processing | Convolution kernels, padding, Sobel/Gaussian kernels | Blur/edge-detect/sharpen filters | Same, templated over kernel |

### 3C. Audio & DSP (70–72) `AUDIO`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 70 | Signal generation | Sine/square wave math, sample rate, WAV header format | Synth → WAV file output | Same, class-based oscillator |
| 71 | Fourier transform | DFT, Cooley-Tukey FFT algorithm, complex numbers | FFT from scratch | Same, templated over sample type |
| 72 | Real-time effects | Delay line buffer, wet/dry mix, streaming processing | Delay/echo effect processor | Same, chainable effect pipeline |

### 3D. Security & Cryptography (73–76) `SEC`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 73 | Hash functions | SHA-256 spec (padding, message schedule, compression) | SHA-256 implementation from spec | Same, `constexpr`-friendly where possible |
| 74 | Symmetric ciphers | AES spec (S-box, key schedule, rounds) or stream cipher | AES (or reduced-round) from scratch | Same, class-based cipher context |
| 75 | Handshake protocols | Diffie-Hellman key exchange, TLS handshake flow | Simulated TLS-lite handshake | Same, OOP protocol state machine |
| 76 | Sandboxing | `seccomp-bpf` filters, syscall allowlisting, capabilities | Syscall filter/sandbox | Same, RAII sandbox guard |

### 3E. Machine Learning & Numerical (77–80) `ML`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 77 | Linear algebra | Matrix multiplication, transpose, dot product, norms | Matrix/vector library from scratch | Same, templated + operator overload |
| 78 | Gradient descent | MSE loss, partial derivatives, learning rate | Linear regression trained by hand | Same |
| 79 | Neural networks | Forward pass, backprop, sigmoid/ReLU, chain rule | MLP+backprop from scratch, no libs | Same, class-based `Layer`/`Network` |
| 80 | Classical ML | Entropy/Gini splitting, or Bayes theorem | Decision tree or naive Bayes classifier | Same |

### 3F. Trading & Quant Finance (81–85) `TRADE`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 81 | Order book | Price-level data structure, bid/ask sides | Order book: add/cancel/modify | Same, class-based, benchmark ops/sec |
| 82 | Matching engine | Price-time priority, matching algorithm | Price-time priority matching engine | Same |
| 83 | Market data normalization | Tick parsing, timestamp handling, normalization | Feed handler parsing+normalizing ticks | Same |
| 84 | Backtesting | Historical replay, strategy interface, PnL calc | Backtest engine vs a strategy | Same |
| 85 | Options pricing | Black-Scholes formula, Monte Carlo, normal sampling | Black-Scholes + Monte Carlo pricer | Same |

### 3G. Blockchain (86–87) `CHAIN`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 86 | Merkle trees & blocks | Hash tree construction, Merkle proof verification | Merkle tree + block structure | Same |
| 87 | Proof of work | Nonce search, difficulty target, chain validation | PoW miner + chain validation | Same |

### 3H. Embedded & Robotics (88–90) `EMBED`
| Day | Concept | Topics To Learn First | C Project | C++ Project |
|---|---|---|---|---|
| 88 | Bare-metal patterns | Memory-mapped I/O concept, cooperative scheduling | GPIO/blink sim + RTOS-lite scheduler | Same |
| 89 | Control systems | PID math (P/I/D terms), feedback loops | PID controller simulation | Same |
| 90 | Sensor fusion | Kalman filter math (predict/update), noise modeling | Kalman filter on noisy sensor data | Same |

---

## Phase 4 — Capstones (Days 91–100)

**Capstone A (91–95): Low-Latency Trading System** — `TRADE` `CONC` `NET`
Topics: integrating order book+matching engine+feed handler+risk checks+backtester, latency measurement methodology (tick-to-trade), end-to-end system architecture.
Build full C and full C++ versions, benchmark latency head-to-head.

**Capstone B (96–100): pick one** — topics depend on track
- **Game** `GAME`/`GFX`: ECS architecture, physics integration, hook up Day 66–68 renderer, game loop design
- **Security** `SEC`: network scanning techniques, intrusion-detection signatures, hook up Day 73–76 crypto work
- **ML** `ML`: anomaly detection on streaming data, hook up Day 77–80 work to Day 83's market-data stream

Both: full README, architecture diagram, tests, CI. Capstone A adds a written C vs C++ latency comparison.

---

## Daily Ritual
1. Study the "Topics To Learn First" column before writing code — don't code-and-google, learn the concept cold first.
2. Build C version — feel where language gives you nothing.
3. Build C++ version — see what abstraction buys, what it costs.
4. 3–5 lines: what broke, what you'd ship.
5. Commit both.

## Reference Material
- *C Programming: A Modern Approach* — K.N. King
- *Effective Modern C++* — Scott Meyers
- *C++ Templates: The Complete Guide* — Vandevoorde/Josuttis/Gregor
- *The Linux Programming Interface* — Michael Kerrisk (Phase 2)
- *Crafting Interpreters* — Robert Nystrom (Phase 3A)
- *Physically Based Rendering* (relevant chapters) — Phase 3B
- *Trading and Exchanges* — Larry Harris (Phase 3F concepts)
- *Mastering Bitcoin* (protocol chapters) — Phase 3G
- godbolt.org — daily