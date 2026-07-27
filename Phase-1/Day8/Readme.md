# Day 8 — `mmap` (Phase 1) `DB`

## What
Memory-mapped file I/O vs plain `read()`-based I/O. C and C++ versions. Goal: understand page-backed virtual memory mapping, prefetch hints, and prove (with real timing) that mmap beats a naive read+buffer grep on large files.

On Windows (no POSIX `mmap`) — used Win32 equivalents:
- `mmap`/`munmap` → `CreateFileMappingA` + `MapViewOfFile` / `UnmapViewOfFile`
- `madvise` → `PrefetchVirtualMemory`

## Files
- `shared_memory.c` — mmap-backed grep via Win32 file mapping API, line-numbered match output, prefetch hint before scan
- `not_shared.c` — plain `fopen`/`fread` into heap buffer, same grep logic, for comparison
- `shared_memory.cpp` — same mmap approach, RAII-wrapped (`MappedFile` class), move-only semantics
- `not_shared.cpp` — same read approach, RAII-wrapped (`FileBuffer` class), copyable (plain heap buffer, no OS handle)
- `Logs.txt` — raw notes from the day

## Build

C:
```
gcc -O2 shared_memory.c -o shared_memory
gcc -O2 not_shared.c -o not_shared
```

C++:
```
g++ -O2 shared_memory.cpp -o shared_memory_cpp
g++ -O2 not_shared.cpp -o not_shared_cpp
```

## Design

**mmap wrapper.** `MappedFile` owns `hFile_`, `hMap_`, `data_` — dtor calls `UnmapViewOfFile`/`CloseHandle` unconditionally via `release()`. Copy disabled (handle can't copy cleanly), move enabled (source nulled out after transfer). Throws on any Win32 failure instead of returning null — caller catches `std::runtime_error`.

**read wrapper.** `FileBuffer` owns a `std::vector<char>` — no OS handle, so copy is safe and left `= default`. This asymmetry (move-only vs copyable) is the actual lesson: ownership semantics should match what resource you're wrapping, not be applied uniformly.

**Prefetch hint.** `PrefetchVirtualMemory` called right after `MapViewOfFile` succeeds, before first touch — Windows' answer to `madvise(MADV_WILLNEED)`. In the C version this call is ordered *after* the null-check on `data`, so it's never reached with a bad pointer.

**Grep.** Line-tracked match printer, not just a counter — real grep prints `line_no: line text`. Match advance is non-overlapping (`ptr += target_len` on hit) — doesn't catch overlapping occurrences, noted as a known limitation, not fixed.

**Benchmark.** `QueryPerformanceCounter`/`QueryPerformanceFrequency` wraps the `grep_buffer` call in all four binaries — timing isolates the scan, not file-open/map overhead.

## Results
```
mmap grep (C):          <5.842, cold> / <8.183 in ms, warm>
read() grep (C):        <7.233, in ms, cold> / <6.914 in ms, warm>
mmap grep (C++ RAII):   <615.948 in ms, cold> / <404.859 in ms, warm>
read() grep (C++ RAII): <812.475 in ms, cold> / <413.692 in ms, warm>
```
**Not filled in yet.** Current `Logs.txt` says mmap "outperformed... by a significant amount" — no numbers. That's not a result, that's a vibe. Run all four, cold (first run after reboot/cache clear) and warm (immediate rerun), record actual ms, fill this table before marking the day closed.

## Notes / what broke

**`main(int argc, char *argv)` → `char *argv[]`** — original signature wrong, wouldn't compile clean.

**Silent empty-file case** — `filesize == 0` originally returned `NULL` with no error message; fixed to print an explicit "broken file" error so caller can tell empty-file apart from a real API failure.

**Prefetch-before-null-check ordering bug** — first draft called `PrefetchVirtualMemory` before checking `data == NULL`, meaning a failed `MapViewOfFile` could pass a null `VirtualAddress` into prefetch. Fixed by reordering: prefetch only runs after the null check confirms mapping succeeded.

**No line tracking initially** — first grep version only counted matches, didn't report which line — not actually "grep," just "count." Added line-start tracking + `memchr` to find line end for printing.

## Takeaway
mmap defers the cost of loading data into page faults triggered on first touch, instead of paying it all up front like `read()` into a heap buffer. That should win on large files with sparse/reused access, and lose (or tie) on small files / pure single-pass sequential scans where `read()`'s upfront cost is already minimal. Whether that's actually true here isn't proven yet — no numbers logged. Don't take "it felt faster" as the result.

## Phase 1 status: day 8 NOT closed
Missing: actual benchmark numbers (cold + warm, all 4 binaries) in the Results table above. Code builds, logic reviewed, semantics correct — but day isn't done until numbers are in and compared, per the daily ritual.

## Todo next
- Day 9 — Buddy allocator, `EMBED` domain: buddy allocator for constrained-memory sim.