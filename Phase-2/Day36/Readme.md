# Day 36 — Coroutines (`ucontext.h` / C++20 `co_yield`)

## What
NPC scripting demo in C and C++, both cooperative coroutines pausing/resuming mid-function instead of running start-to-finish. C version: raw `ucontext_t` context switch per NPC, manual stack, `swapcontext` yield points. C++ version: same NPC script logic rewritten with C++20 `co_yield`, wrapped in a minimal hand-rolled `Generator<T>` class.

## Files
- `NPCscripting.c` — `NPC` struct (own `ucontext_t` + fixed stack + name + done flag), `npc_yield()` swaps back to main, `npc_script()` runs wake/move/attack/done sequence one `swapcontext` at a time
- `npc_coro.cpp` — `Generator<T>` template (`promise_type` w/ `initial_suspend`/`final_suspend`/`yield_value`/`return_void`), `npc_script()` coroutine `co_yield`s each action string
- `context.c` — small standalone before NPC version, plain two-context step1/step2 swap demo, proof-of-concept before wiring up multiple NPCs

## Build
```
gcc -D_XOPEN_SOURCE=700 -O2 -Wall -Wextra NPCscripting.c -o npc_c.exe
g++ -std=c++20 -O2 -Wall -Wextra npc_coro.cpp -o npc_cpp.exe
```
(`ucontext.h` Linux-only — ran under WSL, same as Day 35.)

## Run
```
./npc_c.exe
./npc_cpp.exe
```
Both print tick-by-tick interleaved NPC actions, one step per NPC per tick.

## Design

### C — raw `ucontext` context switch
- Each `NPC` owns its own `ucontext_t` + fixed-size stack array (no malloc — avoided pointer/array type mismatch bug from first draft)
- `current_npc` global index — hack to let `npc_script()` know which NPC it's running as, since `makecontext` only takes `int` args cleanly and passing a struct pointer through is more setup than it's worth for 3 NPCs
- `uc_link` set to `ctx_main` per NPC — when `npc_script()` falls off the end (no manual return-swap needed), control returns to main automatically
- Main loop ticks all NPCs once per iteration, `swapcontext(&ctx_main, &npcs[i].ctx)` resumes each NPC exactly where its last `npc_yield()` paused it — full local state preserved (stack, `self` pointer, position in function)

### C++ — `Generator<T>` over C++20 coroutines
- `promise_type` is the contract compiler needs: `get_return_object`, `initial_suspend`/`final_suspend` (both `suspend_always` — no auto-run before first `.next()`, no cleanup race after last), `yield_value` stashes value, `unhandled_exception` terminates rather than swallowing
- `npc_script()` written as flat top-to-bottom function w/ `co_yield` at each action — compiler builds the state machine, no manual stack, no global index hack
- Main loop drives via `.next()` per NPC per tick, same interleaved-tick shape as C side, wrapped behind `std::optional<T>` so a finished coroutine returns `nullopt` cleanly instead of needing a separate "done" flag check first

## Correctness notes
- First draft had `n->stack = malloc(...)` against a fixed-size array member — compile error, fixed size array can't be reassigned. Dropped malloc, used stack array directly
- First draft passed `idx` into `makecontext(..., 1, idx)` while `npc_script` took zero params — signature mismatch, undefined behavior. Fixed to zero-arg call, `current_npc` global read inside instead
- Redundant manual `swapcontext` call written after `self->done = 1` — dead weight, `uc_link` already handles return-to-main when function falls off the end. Removed
- C++ side: loop reports one extra harmless "tick" after all NPCs finish (finished-check runs once more before `any_active` flips false) — cosmetic only, not a correctness bug

## Results
- C version: 3 NPCs (Goblin/Archer/Wizard) run in lockstep, 4 ticks to completion, each pausing/resuming exact mid-function spot verified via print order
- C++ version: identical output shape, one extra trailing tick line, confirms `co_yield` state machine preserves position same as manual `ucontext` swap
- Side-by-side comparison: C needs manual stack alloc, `ucontext_t` bookkeeping, global index hack to fake per-instance context; C++ hides all of it behind `promise_type` contract — same pause/resume semantics, compiler-generated state machine instead of hand-rolled one

## Todo next
- Extend NPC script w/ branching (`co_yield` inside `if`/loop) — check state machine handles conditional resume points correctly
- Try passing values *into* coroutine via `co_await` (two-way communication), not just `co_yield` out
- Move to Day 37: actor model — message queues, mailbox pattern, async dispatch (`TRADE` — order routing)