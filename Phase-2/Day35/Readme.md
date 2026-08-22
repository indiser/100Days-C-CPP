# Day 35 — Async I/O (`epoll`)

## What
Event-loop market-data-style feed handler in C and C++, both edge-triggered (`EPOLLET`). C version: raw `epoll_wait` loop dispatching on `data.ptr` sentinel (`NULL` = listen fd, else per-conn state). C++ version: same architecture wrapped as a `Reactor` class with polymorphic `EventHandler` dispatch.

## Files
- `async.c` — single-file event loop, `struct conn_state` per connection, manual epoll_ctl ADD/MOD/DEL
- `async.cpp` — `Reactor`, `EventHandler` interface, `AcceptHandler` / `ClientHandler` concrete handlers

## Build
```
gcc -O2 -Wall -Wextra async.c -o async_c.exe
g++ -O2 -std=c++17 -Wall -Wextra async.cpp -o async_cpp.exe
```
(Ran under WSL — `epoll` is Linux-only, no native Windows path.)

## Run
```
./async_c.exe
./async_cpp.exe
```
Test: `curl http://localhost:8080/` or hit with `wrk`/`nc`.

## Design

### C — raw event loop
- Listen fd + all conn fds set non-blocking via `fcntl`, registered `EPOLLET`
- `data.ptr` trick: `NULL` sentinel identifies listen fd, real conns carry heap-allocated `conn_state*` — avoids separate fd→state lookup table
- Accept loop drains until `EAGAIN` (mandatory under ET — one edge, could be N pending conns)
- Read loop drains until `EAGAIN` per readable event, same ET reasoning
- Write path: buffer response in `conn_state`, arm `EPOLLOUT` via MOD, drain on writability, close only after full write or hard error — handles partial-write-on-full-socket-buffer case
- `EPOLLRDHUP | EPOLLHUP | EPOLLERR` checked first, unconditional teardown

### C++ — `Reactor`
- `EventHandler` abstract interface: `handle_read/write/error`, `get_fd`
- `Reactor` owns epoll fd + `unordered_map<int, shared_ptr<EventHandler>>`, drives `run()` loop, exposes `register_handler`/`modify_handler`/`remove_handler`
- `AcceptHandler` and `ClientHandler` concrete implementations, same drain-till-EAGAIN and buffered-write logic as C side
- `run()` rechecks the handler map after `handle_read()` before calling `handle_write()` on the same fd — guards against acting on a handler that `handle_read()` already tore down in the same event

## Correctness notes
- Everything is `EPOLLET` — went straight to edge-triggered instead of level-triggered first pass. Riskier (missed drain = event never refires) but both drain loops verified correct against `EAGAIN`
- Write is never a single blind `write()` call — always loop + track `write_pos`, since non-blocking socket can accept a partial write under load
- Double-dispatch bug caught and fixed: without the re-check, `handle_read()` erasing the handler on error then `run()` still calling `handle_write()` on the freed/stale entry
- C side: no NULL-check on `calloc` for `conn_state` yet — OOM path unhandled, noted as gap, not yet crashed in practice

## Results
- Both versions correctly served HTTP `200` for single-shot `curl` requests, closed after response per `Connection: close`
- Verified accept-storm handling (rapid parallel connects) drains fully per epoll_wait wake, no dropped conns during manual test
- Confirmed partial-write path works do not lose bytes when forcing small buffer / high concurrency

## Todo next
- Run actual stress test: `wrk -t8 -c2000 -d30s`, capture `perf stat` + `mpstat` during run, compare C vs C++ overhead
- Add `calloc` NULL-check in C `conn_state` allocation
- Move to Day 36: coroutines via `ucontext.h` (C) and C++20 `co_await`/`co_yield`