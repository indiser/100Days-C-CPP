# Day 37 — Actor Model (Message Queues / Mailbox Pattern / Async Dispatch)

## What
Order-routing actor system in C and C++, both message-passing not shared-state. C version: single actor consuming lock-free SPSC-style queue, three message types dispatched by tag. C++ version: full router+worker actor pool, symbol-hashed dispatch across N workers, isolated per-worker state, no lock ever touches shared data, backpressure-safe mailbox, stress-tested under concurrent producers.

## Files
- `ActorModel.c` — `Message` tagged union (`NEW_ORDER`/`CANCEL_ORDER`/`POISON_PILL`), Vyukov-style bounded lock-free MPSC queue (`queue_push`/`queue_pop`, sequence-number slots, CAS on head/tail), one `Actor` struct wrapping `pthread_t` + queue pointer, `actor_worker()` pop loop dispatch on tag
- `ActorModel.cpp` — `Message` as `std::variant`, `Mailbox` class (mutex + `condition_variable`, blocking push/pop, capacity-bounded — no silent drop), abstract `Actor` base (owns thread + mailbox, pure virtual `process_message`), `OrderBookWorkerActor` (isolated `processed_count`, no mutex needed — state never shared), `OrderRouterActor` (owns worker pool, routes by `symbol % num_workers`), multi-producer stress harness (4 producer threads × 10k orders)

## Build
```
gcc -O2 -Wall -Wextra -pthread ActorModel.c -o actor_c.exe
g++ -std=c++20 -O2 -Wall -Wextra -pthread ActorModel.cpp -o actor_cpp.exe
```

## Run
```
./actor_c.exe
./actor_cpp.exe
```
C prints NEW_ORDER/CANCEL_ORDER/shutdown lines for the 3 hand-pushed messages. C++ prints per-worker processed counts after 40,000 concurrently-produced orders drain and shutdown propagates router → all workers.

## Design

### C — single actor, lock-free mailbox
- Lock-free bounded queue (Dmitry Vyukov design): each slot carries its own `sequence` counter, CAS on `tail`/`head` only, `diff` check tells producer/consumer whether slot is ready — no locks anywhere in the hot path
- Messages heap-allocated, tagged union (`MessageType` + `union` of payload pointers), actor frees payload+envelope after processing — ownership transferred producer → actor on push
- One actor only, no router. `main()` pushes 3 messages serially, actor pops in order, exits on poison pill

### C++ — router + worker pool
- `Mailbox` blocks on full (`cv.wait` on `size < capacity`) instead of dropping — no order ever silently vanishes under backpressure
- `OrderRouterActor` holds `vector<unique_ptr<OrderBookWorkerActor>>`, hashes `symbol % workers.size()` to pick target, forwards message — router never touches worker state directly, only sends messages across the boundary
- Each `OrderBookWorkerActor` owns `processed_count` with zero synchronization — safe only because no other thread ever touches it, which is the actual point of actor model over lock-based sharing
- Poison pill fans out: router receives one, forwards one to every worker, each worker prints its own final count and exits
- Stress test: 4 producer threads hammer the router's mailbox concurrently with 10,000 orders each — proves the mailbox lock is correct under real contention, not just single-writer

## Correctness notes
- First cut of C version used `usleep(100)` spin-wait on empty queue — burns CPU nonstop even idle. Switched to `sched_yield()`; real fix would be futex/semaphore wake-on-push, noted as follow-up, not done here
- First cut of C version ignored `queue_push` return value — full queue meant a message silently vanished with no log, no drop counter. Not acceptable in a trading path. **Still unfixed in this version** — return value checked nowhere in `main()`
- C version never proven under concurrent producers — only one thread ever calls `queue_push`, so the lock-free MPSC claim is unverified. Needs a multi-producer stress test like the C++ side got, or the "lock-free" label is just asserted, not demonstrated
- C++ first cut had a destructor-order bug: `OrderRouterActor`'s `workers` vector (member, destroyed first) could tear down worker threads while the router's own worker thread was still mid-dispatch to them — race between "router forwarding poison pill" and "worker object already destructing." Fixed by adding explicit `Actor::join()` and calling `router.join()` in `main()` before `router` goes out of scope, guaranteeing router thread is fully done (all forwarding complete) before workers destruct
- C++ side correctly keeps all inter-actor communication as copied/moved messages — no actor ever receives a pointer into another actor's live state, which is the actual test of "did you build actor model or just a job queue"

## Results
- C: 1 actor, 3 messages, correct ordering, clean shutdown on poison pill — but single-actor, single-producer, no routing, unverified concurrency claim
- C++: 4 workers, 4 concurrent producers, 40,000 orders routed by symbol hash, isolated per-worker counters sum correctly, graceful cascading shutdown, no crash/race observed
- Should not be called "done" on log evidence alone — build ran and printed clean once. Concurrency bugs hide behind clean single runs constantly. Rerun C++ under `-fsanitize=thread` before trusting it; nothing here proves absence of a race, only absence of one so far

## Todo next
- Fix C version: check `queue_push` return, add real multi-producer stress test, swap spin/yield for futex or semaphore wake
- Run C++ stress test under ThreadSanitizer (`-fsanitize=thread`) — clean stdout is not proof of correctness
- Move to Day 38: producer-consumer at scale — multi-stage real-time audio pipeline (`AUDIO`)