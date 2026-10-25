---
nav_exclude: true
---
# Mrm text-vector bounds

Mrm Char8 and CString vector setters now reject negative counts and counts
whose complete text-vector descriptor cannot fit in the format's 16-bit
`MrmSize` field. Zero-count vectors and valid existing counts retain their
behavior, while wrapped `vecsiz` values are prevented before record allocation.

The source-level regression is `tests/automated/check-mrm-text-vector-bounds.py`.
