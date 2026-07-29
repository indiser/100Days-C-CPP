# Day 14 — Endianness / Binary Formats (Phase 1) `GFX`

## What
Byte-exact BMP parser+writer. Load header+pixels, invert colors, write back out. Built in C (`byteMath.c`) and C++ (`byteMath.cpp`, `BMPImage` class).

## Files
- `byteMath.c` — `BMPHeader` packed struct (`#pragma pack(1)`), `openBMP`/`verifyBMP`/`invertBMP`/`writeBMP`, raw `fread`/`fseek`
- `byteMath.cpp` — same header layout, wrapped in `BMPImage` class (`open`/`save`/`invertColors`/`isValid`), `std::vector<uint8_t>` for pixel buffer
- `Logs.txt` — day notes

## Design
Struct-overlay approach (not manual field-by-field byte reads) — relies on `#pragma pack(1)` to kill padding so struct matches file layout exact. Works, but skipped the "read raw bytes by hand" exercise that actually teaches endianness — see bugs below, this is why it bit you.

Row padding handled correctly: `(4 - (width*3) % 4) % 4`, skipped via `fseek`/`seekg` after each row. Top-down vs bottom-up (negative height) handled via `abs(height)` for loop bound, sign not otherwise used.


None of these are exotic. They're the standard "struct-overlay BMP parser" failure list — good that you built it working end to end, but "loads my one test image fine" is not "byte-exact." Byte-exact means it handles the spec, not just your one file.

## Notes / what broke
Logs are thin again — third day running near-identical "it worked, all good" entry with no mention of the 7 issues above, because none were tested for. You're validating against one image that happens to be 24-bit, uncompressed, top-down or whatever it is, and calling it done. That's the same pattern flagged on Day 13: happy-path round-trip isn't understanding the format.

## Todo next
- Test against hand-built 2×2 known-good BMP, hex-dump, verify exact byte match — not eyeball-the-output
- Test against non-24bpp and RLE-compressed BMP to confirm rejection path actually rejects
- Portable paths (relative or CLI arg) so this repo builds outside your machine