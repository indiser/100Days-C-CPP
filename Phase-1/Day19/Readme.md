# Day 19 — Tagged Unions `GAME`

## What
Store different item types (Weapon, Armor, Potion) in one type — pick right member by tag instead of separate arrays per type. C way: `enum` tag + raw `union`, manual `switch` dispatch. C++ way: `std::variant`, type-safe, compiler enforces every case handled via `std::visit` + overload set.

## Files
- `tagged_unioon_inventory.c` — manual tagged union: `ItemType` enum tag, raw `union { Weapon; Armor; Potion; }` inside `Item` struct, `switch(item.tag)` in `Process()` picks correct member. Nothing stops reading wrong member if tag's wrong — trust, not enforcement.
- `tagged_union_inventory.cpp` — `using Item = std::variant<Weapon, Armor, Potion>`, dispatch via `std::visit` + `overloaded` trick (`template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };` + deduction guide). Compiler errors if a type's lambda missing — no silent wrong-read possible.
- `Logs.txt` — day log

## Build

C:
```
gcc tagged_unioon_inventory.c -o inv_c
./inv_c
```

C++:
```
g++ -std=c++17 tagged_union_inventory.cpp -o inv_cpp
./inv_cpp
```

## Design

**Tag says which member is live — union itself doesn't know.** `Item` struct holds `ItemType tag` next to the `union`. Read `data.weapon` when tag says `ITEM_POTION` — no crash, no warning, just garbage. Union's just a shared memory slot, same address reinterpreted as whatever type asked for it. Tag discipline is on you.

**`std::variant` makes the tag part of the type.** No manual enum, no manual switch. Variant tracks active alternative internally, `std::visit` forces a handler for every alternative at compile time. Miss a case in the `overloaded` set — build fails, not a silent wrong-read at runtime.

**`overloaded` trick is a lambda-to-visitor adapter.** `std::visit` wants one callable with `operator()` overloaded per type; three separate lambdas aren't that. `overloaded` inherits from all three, pulls in all three `operator()` via `using`, so overload resolution picks the right lambda per alternative. Deduction guide (`overloaded(Ts...) -> overloaded<Ts...>`) lets aggregate construction infer the template args from the lambdas passed in.

**Union shares memory, size reflects the biggest member.** `sizeof(inventory[0].data)` — 8 bytes, matches `Weapon` (`int` + `float`), the largest of the three payloads. `sizeof(Item)` — 12 bytes, tag (4 bytes int) + union (8 bytes), no padding needed here. `std::variant`'s `sizeof(Item)` also lands at 12 — union bytes + internal index, same shape as the manual version, just compiler-managed.

## Results
`tagged_unioon_inventory.c` run:
```
Item 1:
Damage: 50
Durability: 95.50

---
Item 2:
Defense: 20
Weight: 15

---
Item 3:
Heal Amount: 100
IS Elixer: YES

---
Size of Item: 12 bytes
Size of union: 8 bytes
```

`tagged_union_inventory.cpp` run:
```
Item 1: Weapon | Damage: 50 | Durability: 95.5
Item 2: Armor  | Defense: 20 | Weight: 15
Item 3: Potion | Heal: 100 | Elixir: YES
---
Size of Item variant: 12 bytes
```
Both versions same output shape, same size — variant costs nothing extra over raw union here, just adds compile-time safety.

## Notes / what broke
`default` case in C `switch` — dead code today since only 3 enum values exist, but keeps future-proofed against a 4th `ItemType` added without updating `Process()`. `std::variant` doesn't need this at all — new alternative added to the `using Item = std::variant<...>` list, every `std::visit` call site missing a handler for it fails to compile. Enforcement moves from "hope you remember" to "build breaks."

Small naming slip in the C file — `isElixer` param vs `isExiler` struct field, cosmetic only, both compile and run fine, just inconsistent spelling carried through.

## Todo next
- Add a 4th `ItemType` (e.g. `ITEM_SCROLL`) to both versions — confirm C silently does nothing new (falls into `default`) while C++ refuses to compile until every `std::visit` call site is updated
- Try `std::get<Weapon>(item)` / `std::get_if` on a variant holding a different alternative — confirm throw vs null behavior
- Benchmark raw union dispatch vs `std::visit` — variant's supposed to compile down to a jump table, worth checking codegen