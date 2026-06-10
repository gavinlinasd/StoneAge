# StoneAge

StoneAge 8.0 source code I found online. 
Trying to bring the code from 2005 to 2018 by giving it new life and run it on Cloud.

## Status (2026 modernization)

Both servers build clean on gcc 13 / Ubuntu 24.04 (x86-64) and run:
`saac` (account server, MariaDB) and `gmsv` (game world server) boot,
authenticate to each other, load the full world data, and accept TCP
connections. See [BUILDING.md](BUILDING.md) for build/run instructions.

This branch was redone from the pristine 2005 source (commit
`6c272e3`). Highlights of what was needed:

- **Encoding archaeology**: the sources carried three text strata -
  original Japanese comments in EUC-JP, that Japanese *fossilized* into
  GBK mojibake by a historical Big5→GB2312 conversion, and genuine
  Chinese comments in GBK. Sources are UTF-8 now: ~2,600 fossil comment
  lines were first restored to Japanese by reversing the exact
  historical conversion chain (`民尼永弁` → `チェック`) and then, with
  the rest of the Japanese comments, translated to Simplified Chinese
  (`チェック` → `检查`). Game data deliberately stays GB18030 - see
  BUILDING.md for why.
- **64-bit correctness**: pointers stored in `int` work-table slots,
  a function returning a pointer to a stack array (the boot segfault),
  `int` overflow in the 5 GB memory-pool arithmetic, and a stack smash
  from a 15-element array swept by a 30-iteration loop.
- **Modern toolchain reality**: pre-C17 common symbols had silently
  merged ~30 duplicate globals across translation units - including one
  variable whose definition was compiled out by a wrong `#ifndef` and
  whose storage existed *only* because of common-symbol merging.
- The 1998 recursive makefiles were replaced with small non-recursive
  ones with dependency tracking; warnings that indicate 64-bit breakage
  are promoted to errors.
