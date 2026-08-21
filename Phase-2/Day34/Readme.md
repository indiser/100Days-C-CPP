# Day 34 — Real Thread Pools

## What
Build fixed-size worker thread pool in C and C++. C version: raw job queue via `pthread`, mutex + condition variable, fire-and-forget tasks. C++ version: same architecture wrapped in RAII class, `enqueue()` returns `std::future<T>` via `std::packaged_task`.

## Files
- `C/JobSystem.c` — `threadpool_t`, circular buffer job queue, manual lifecycle (`create`/`add`/`destroy`)
- `CPP/JobSystem.cpp` — `ThreadPool` class, templated `enqueue`, future-based result return

## Build

C:
```
gcc -O2 -pthread JobSystem.c -o jobsystem_c.exe
```

C++ (must use g++ — needs libstdc++ for thread/future/chrono):
```
g++ -O2 -std=c++17 -pthread JobSystem.cpp -o jobsystem_cpp.exe
```

## Run
```
./jobsystem_c.exe
./jobsystem_cpp.exe
```

## Design

### C — `threadpool_t`
- Circular buffer (`head`/`tail`/`count`) holding `task_t { func, args }`
- Worker loop: lock, predicate-wait (`count == 0 && !shutdown`) on `pthread_cond_wait`, pop under lock, unlock before calling `task.func` (avoid holding lock during work)
- `threadpool_add`: rejects on full queue or shutdown-in-progress, returns `-1` — caller responsible for handling (drop + free)
- `threadpool_destroy`: set shutdown flag under lock, `pthread_cond_broadcast`, join all threads, then destroy mutex/cond, free buffers
- All `malloc` paths NULL-checked, `pthread_create` return checked — partial-failure path cleans up already-spawned threads via `threadpool_destroy`

### C++ — `ThreadPool`
- `std::queue<std::function<void()>>` job queue, `std::mutex` + `std::condition_variable`
- `enqueue<F, Args...>` wraps callable in `std::packaged_task`, held in `shared_ptr` (packaged_task non-copyable, `std::function` requires copyable — shared_ptr bridges that), returns `std::future<return_type>` via `std::invoke_result`
- Throws `std::runtime_error` on enqueue after `stop` set, instead of silent drop (stronger contract than C side's `-1` return)
- Destructor: lock, set `stop`, unlock, `notify_all`, join every worker — RAII, no manual destroy call needed

## Correctness notes
- Predicate-wait loop on both sides guards spurious wakeup — never trust a single `cond_wait` return
- Shutdown flag checked *again* inside lock after wake, before pop — no missed final task, no exit-with-work-left
- C++ worker drops lock before invoking `task()` — same reasoning as C side, don't serialize execution behind the queue lock
- Exception thrown inside a C++ task body is captured by `packaged_task`, stored in the future, rethrown on `.get()` — not yet manually verified with a throwing lambda, todo below

## Results
Both pools run 4 workers against 8 queued jobs, confirmed:
- All 8 tasks execute exactly once, output interleaved (real concurrency, not serialized)
- `threadpool_destroy` / `~ThreadPool()` block until all queued work drains — no `sleep()` needed as artificial sync in either `main`
- No leaked threads (all joined), no leaked task args (dropped tasks freed on C side)

## Todo next
- Manually verify exception propagation: enqueue a throwing lambda in C++ version, confirm `.get()` rethrows
- Compare throughput C vs C++ pool under heavier load (1000+ short tasks) — quantify `std::function`/`packaged_task` overhead vs raw function-pointer dispatch
- Move to Day 35: `epoll`-based async I/O, non-blocking sockets