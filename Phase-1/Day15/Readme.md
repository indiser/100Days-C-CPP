# Day 15 — Signal Handling (Phase 1) `SYS`

## What
Crash handling via signals — SIGSEGV catch+backtrace, stack-overflow detection via `sigaltstack`, SIGINT/SIGTERM graceful catch. Built in C (`SFHandler.c`, `SOHandler.c`, `signals.c`) and C++ (`nullPtr_deref.cpp`, `stack_overflow.cpp`).

## Files
- `SFHandler.c` — null-deref SIGSEGV handler, `sigaction`-based, dumps `backtrace_symbols_fd` to stderr, `_exit(1)`
- `SOHandler.c` — stack-overflow detector: `pthread_attr_getstack` for bounds, `sigaltstack` + `SA_ONSTACK` so handler survives blown main stack, `si_addr` range-check to distinguish overflow from ordinary segfault, falls back to `SIG_DFL`+`raise` for non-overflow case
- `signals.c` — SIGINT/SIGTERM catch via `sigaction`, keep-alive loop
- `nullPtr_deref.cpp` — C++ version, RAII-style `CrashHandler` class installs handler in constructor
- `stack_overflow.cpp` — C++ version, `StackOverflowHandler` class, `volatile char buf[1024]` in `cause_overflow()` to defeat tail-call optimization
- `Logs.txt` — day notes

## Design
`sigaction` over `signal()` throughout — no reset-to-default surprise, explicit flags. Stack-overflow path uses `si_addr` from `siginfo_t` compared against `pthread_attr_getstack` bounds (with 4KB guard-page slack) to tell "stack blew up" apart from "random bad pointer" — same signal, different cause, different response. Handler-side work kept to async-signal-safe calls: `write()`, `backtrace()`, `backtrace_symbols_fd()`, `_exit()` — no `printf` in a handler.

## What broke / what's still open
Claimed all handled — checked the files, not fully true, flagging honest:

- `SFHandler.c` / `signals.c` — `struct sigaction sa;` declared but never zero-inited and no `sigemptyset(&sa.sa_mask)` call. `sa_mask` and `sa_flags` are garbage stack values, not `{0}`. Works by luck on your machine, not guaranteed. Fix: `struct sigaction sa = {0};` or explicit `sigemptyset`.
- `signals.c` — `handler()` still writes `13` bytes for an 11-char string (`"I wont die\n"`), same off-by-two bug flagged before, fixed in `SFHandler.c` but not here.
- `SOHandler.c` (C version) — `cause_overflow()` uses plain `char buf[1024]`, not `volatile`. C++ twin fixed with `volatile` to block tail-call optimization; C version still exposed to the compiler collapsing recursion into a loop at `-O1`+ and never actually overflowing. Confirm which opt level this was tested at — if not `-O0`, this may not have crashed the way you think it did.
- Neither `SOHandler.c` nor `stack_overflow.cpp` pre-warm `backtrace()` before the crash — first call can `malloc` internally, risk of deadlock if that malloc lock is already held. Untested edge, not fixed, just not hit yet.
- `sig_stack = malloc(...)` in both SO handlers — no NULL check. Silent garbage `sigaltstack` on OOM.

## Todo next
- Zero-init every `sigaction` struct, no exceptions
- NULL-check `sig_stack` malloc