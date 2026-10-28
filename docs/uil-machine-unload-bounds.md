---
nav_exclude: true
---

# UIL machine-listing unload bounds

Machine-listing offset records use 16-bit lengths and offsets. The unload path
now rejects oversized embedded strings, descending offsets, and record spans
that cannot fit the compact length field before passing them to source-record
construction.

Regression: `tests/automated/check-uil-machine-unload-bounds.py`
