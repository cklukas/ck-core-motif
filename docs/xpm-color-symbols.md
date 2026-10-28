---
nav_exclude: true
---

# XPM color-symbol attributes

The raw XPM-image conversion path now rejects `XpmColorSymbols` with a
nonzero symbol count but no symbol array.  This prevents malformed caller
state from reaching the color-symbol scan and preserves the existing behavior
for empty or fully populated symbol lists.

Regression: `tests/automated/check-xpm-color-symbols.py`
