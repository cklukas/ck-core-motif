---
nav_exclude: true
---
# Input-method area geometry bounds

The input-method integration now clamps shell-height removal and status or
preedit rectangle placement when the IM area is taller than its shell.  This
prevents unsigned `Dimension` subtraction from wrapping shell requests or XIM
area coordinates during provider teardown and reconfiguration.

The separator drawn above the IM area uses the same guarded subtraction and is
saturated at Motif's legacy `Position` boundary, so a very large shell cannot
wrap the separator to an unrelated vertical coordinate. Xlib's instantiate
callback result is normalized to Motif's Boolean state before it is retained.

The source-level regression is `tests/automated/check-xmim-area-bounds.py`.
