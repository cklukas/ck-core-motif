---
nav_exclude: true
---

# UIL root filename metadata bounds

The UIL symbol-table root has fixed 256-byte filename fields for ABI
compatibility. Root creation now copies command-line and expanded filenames
with a terminating bound, preventing an unusually long path from overwriting
adjacent symbol data. Only the stored metadata is truncated; source opening
continues to use the original path.

Regression: `tests/automated/check-uil-root-filename-bounds.py`
