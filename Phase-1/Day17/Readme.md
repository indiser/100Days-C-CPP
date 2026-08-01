# Day 17 — Memory Interception & Leak Tracking `MEM`

## What
Hook malloc/free (C) and operator new/delete (C++) to catch leaks, double frees, bad frees. Compare dynamic symbol interposition (LD_PRELOAD + dlsym/RTLD_NEXT) vs macro redirection (#define malloc trick) vs global operator overload in C++. Reentrancy guard problem hits hard here — tracker itself allocates, causes infinite loop if not guarded.

## Files
- `logger.c` — LD_PRELOAD hook, dlsym(RTLD_NEXT, "malloc"), logs via raw write() not printf (printf allocs, causes recursion)
- `return_addr.c` — same hook pattern, plus __builtin_return_address(0) to grab caller address, id who called malloc
- `Tracker.c` — full C tracker, macro redirection (#define malloc tracked_malloc(sz, __FILE__, __LINE__)), fixed array MAX_ALLOCS 1024, thread_local inside_tracker guard, dump_leaks() on atexit
- `Tracker.cpp` — C++ version, global operator new/delete/new[]/delete[] overloads (sized + unsized), fixed AllocEntry array, mutex + thread_local guard, catches array/single delete mismatch
- `reentrency_guard.cpp` — 3 guard techniques side by side: thread_local flag, RAII TrackerGuard struct, static bootstrap buffer for dlsym bootstrap paradox
- `funcPrepro.c` — side quest, MIN() macro via GNU statement-expr + typeof, safe against double-eval unlike naive #define MIN(a,b) ((a)<(b)?(a):(b))
- `Notes.txt` — raw theory dump: symbol interposition, LD_PRELOAD, RTLD_NEXT, weak attribute, bootstrap paradox, zero-alloc hash table pattern
- `Logs.txt` — day log

## Build

C, preload hook:
```
gcc -shared -fPIC -o logger.so logger.c -ldl
LD_PRELOAD=./logger.so ./target_binary
```

Same for return_addr.c:
```
gcc -shared -fPIC -o return_addr.so return_addr.c -ldl
LD_PRELOAD=./return_addr.so ./target_binary
```

Macro-redirect tracker, standalone:
```
gcc Tracker.c -o tracker -ldl
./tracker
```

C++ operator overload tracker:
```
g++ Tracker.cpp -o tracker_cpp -pthread
./tracker_cpp
```

## Design

**Two hook strategies.** LD_PRELOAD hijacks at link/load time, no source changes needed, works on any binary. Macro redirection (`#define malloc tracked_malloc(...)`) needs recompile but gives free `__FILE__`/`__LINE__` context — preload can't get that without stack walking.

**Reentrancy is the real boss fight.** Tracker calls printf → printf calls malloc → hits hook again → stack overflow. Three fixes tried:
1. `thread_local bool inside_tracker` flag, check at top, bail to real_malloc if already inside
2. RAII guard struct — ctor sets flag, dtor clears it, safe even if exception/early-return hits
3. Bootstrap buffer — dlsym() itself may call calloc before real_malloc pointer is ready, hand out static byte array during that window

**dump_leaks / LeakChecker** — C version registers atexit(dump_leaks), C++ version uses static object dtor (LeakChecker) since C++ has no clean atexit-with-context equivalent for this. Both walk the fixed table and print anything not freed.

**Swap-back removal.** `alloc_table[i] = alloc_table[--alloc_count]` — O(1) removal, don't care about order, avoids shifting whole array on every free.

**BAD FREE / double-free detection.** free() on untracked pointer prints `BAD FREE: %p at %s:%d` instead of silently corrupting. C++ side also flags array/non-array mismatch (`new[]` freed with plain `delete`).

## Results
`Tracker.c` run: p1 freed clean, p2 (20 ints) never freed →
```
LEAKS DETECTED:
Leak 80 bytes at 0x... (Tracker.c:...)
```
`Tracker.cpp` run: `a` deleted clean, `arr` (new int[50]) left leaked, commented out on purpose to prove detection →
```
CPP LEAKS DETECTED:
Leak 200 bytes at 0x... [Array: yes]
```
Confirms both trackers catch real leaks, not just theory.

## Notes / what broke
Printf inside malloc hook = instant infinite recursion, learned this the hard way before adding guards. logger.c comment calls it out directly: don't use printf, use write().

Fixed array size (1024 / MAX_RECORDS) is a real limit — pool exhaustion silently drops tracking past that, no resize since resize = malloc = recursion risk. Fine for a day's exercise, not production-real.

`reentrency_guard.cpp` currently defines `operator new` twice (Technique 1 and Technique 2 both as real overloads) — as-is this won't link, only one can be live at a time. Needs `#if`/`#ifdef` split or comment one out before building for real.

## Todo next
- Swap fixed array in Tracker.c/.cpp for the zero-alloc open-addressed hash table from Notes.txt — O(1) lookup instead of O(N) scan on free
- Add call-stack capture (__builtin_return_address chain) into Tracker.c/.cpp leak reports, not just file/line
- Test macro-redirect version against a header that itself uses malloc (stdlib containers) — likely breaks, need to check