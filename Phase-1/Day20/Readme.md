# Day 20 — Bit-Packed Structures `GAME`

## What
Pack chess board state into raw bits instead of arrays-of-structs. C way: 12 `uint64_t` bitboards (one per piece type), hand-rolled shift/mask ops, macros for set/clear/get. Also side-quest: bitfield struct packing (`unsigned x : N`) and raw shift/mask primitives on a scratch `uint64_t`. C++ way: same bitboard-per-piece design wrapped in a `Board` class, `constexpr` construction, `std::popcount` for piece counting.

## Files
- `bitBoard.c` — 12-bitboard chess board: `init_board`, `print_board` (rank 7→0, overlay support for attack masks), `get_occupied`, `knight_attacks` (file-wraparound guarded via `FILE_A/B/G/H` masks), `push_white_pawns`/`push_black_pawns` (clear-source/set-target pattern, not blind overwrite).
- `bitBoard.cpp` — same board as `Board` class, `std::array<uint64_t,12>` backing store, `constexpr Board()` constructor + `static_assert` proving compile-time init, `std::popcount` for `total_pieces()`, same knight/push logic as member functions.
- `bitfields.c` — scratch file, raw bit primitives on one `uint64_t` (`SET_BIT`/`CLR_BIT`/`IS_BIT_SET` macros), prints 64-bit occupancy as `+`/`.` string. Struct bitfield version commented out — left as reference, not run.
- `bitwise.c` — scratch, single test of `~` (bitwise NOT) on signed int, hex print.
- `BITMASK.txt` — notes: shift/mask/AND/OR/XOR/NOT cheat sheet, mask-construction formula `(1U << N) - 1`.
- `Logs.txt` — day log

## Build

C:
```
gcc bitBoard.c -o board_c
./board_c
```

C++:
```
g++ -std=c++20 bitBoard.cpp -o board_cpp
./board_cpp
```

Scratch files (optional, not part of main deliverable):
```
gcc bitfields.c -o bf && ./bf
gcc bitwise.c -o bw && ./bw
```

## Design

**Bitboard-per-piece-type, not bitfield-per-square.** Considered `struct Square { type:3; color:1; moved:1; }` array-of-64 approach first — real bit-packing, but wrong shape for chess. Real engines use 12 `uint64_t`, one bit per square per piece type. Board occupancy, attack masks, move generation all become single bitwise ops across a whole board at once instead of per-square loops.

**Push move = shift, not per-square update.** `pieces[P] << 8` moves every white pawn up one rank simultaneously. AND against `empty()` stops pawns pushing onto occupied squares. This is where whole thing almost went wrong — first pass did `boards[P] = pushed;`, which replaces board with only the pawns that moved, silently deleting blocked ones. Real fix: backtrack shifted result to find `movers`, clear only those sources, OR in new targets. `pieces[P] &= ~movers; pieces[P] |= pushed;` — proven correct with a blocker test (rook parked in front of e2 pawn, confirmed pawn stays put after push instead of vanishing).

**Knight attacks need file-wraparound guards.** Shifting bits left/right near file A or H wraps into the neighboring rank if unmasked — a1 knight naively shifted right can look like it landed on h-file of a different rank. `FILE_A/B/G/H` masks strip bits that would wrap before combining shift directions.

**`constexpr` board construction — proved, not claimed.** C++ version's `Board()` constructor is `constexpr`; backed it with `static_assert(compile_time_board.occupied() == 0xFFFF00000000FFFFULL, ...)` so build itself fails if compile-time init breaks. Claiming zero-runtime-cost construction without a test enforcing it is just a keyword, not a demonstrated property.

**Bitfield struct packing kept separate as scratch (`bitfields.c`), not folded into board.** Bitfield layout is implementation-defined across compilers — fine for local scratch state, wrong choice for a data structure meant to be reasoned about precisely (board). Raw `uint64_t` + explicit shift/mask macros used for board instead, portable and byte-exact by construction.

## Results
`bitBoard.c` / `bitBoard.cpp` run (both):
```
--- INITIAL BOARD ---
8 r n b q k b n r
7 p p p p p p p p
6 . . . . . . . .
5 . . . . . . . .
4 . . . . . . . .
3 . . . . . . . .
2 P P P P P P P P
1 R N B Q K B N R
  a b c d e f g h

--- WHITE KNIGHT ATTACKS (*) ---
... * . * . * . * ... (a3/c3/d2 style spread from b1/g1 knights)

--- BLOCKER ADDED AT e3 ---
(black rook placed at e3, sq 20)

--- WHITE PAWNS PUSHED (e2 blocked) ---
(all pawns advance to rank 3 except e2, which stays — rook blocks it)

--- BLACK PAWNS PUSHED ---
(all black pawns advance to rank 6)
```
Both versions produce identical board states at every step — confirms C hand-rolled version and C++ class version implement same logic, just different cost/ergonomics.

## Notes / what broke
Push-move overwrite bug — first version replaced whole pawn bitboard with only the pushed subset instead of merging, silently deleting blocked pawns. Caught by deliberately placing a blocker and checking it stayed. Would not have caught this on starting position alone (nothing blocked, bug invisible).

Overlay/glyph priority bug in `print_board` — attack-mask overlay (`*`) was drawn even on squares with a piece already on them, hiding the piece. Fixed by checking piece glyph first, only falling back to `*` on empty squares.

C version originally lagged C++ by a full turn (push bug unfixed, black push missing, knight attacks unwired) — C++ got the fixes first since class structure made it easier to iterate, then ported same fixes back to C. Good reminder: don't let one language's momentum leave the other half-finished.

## Todo next
- Add sliding-piece attacks (rook/bishop) — can't use simple shift-and-mask like knight, needs ray-tracing with blocker detection (classic bitboard hard part, magic bitboards eventually)
- Add `pop_lsb` using `std::countr_zero` (C++) / `__builtin_ctzll` (C) to iterate set bits instead of looping all 64 squares every time — real perf move engines use
- King/castling flags — revisit `GameState` bitfield struct from Square/GameState side-experiment, wire into actual game state tracking instead of leaving it as decoration