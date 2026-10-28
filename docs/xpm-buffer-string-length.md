---
nav_exclude: true
---

# XPM in-memory string length

The buffer-backed `xpmGetString` path now checks the scanned span before
converting it to the library’s `unsigned int` length and allocating the
record. Oversized in-memory records return `XpmNoMemory` safely.

Regression: `tests/automated/check-xpm-buffer-string-length.py`
