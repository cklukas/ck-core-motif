---
nav_exclude: true
---
# Generic Text substring bounds

`_XmTextGetSubstring()` now rejects null buffers, negative positions/counts,
overflowing position ranges, and non-positive capacities before reading the source.  Its output checks
reserve the terminating NUL and measure wide output in destination elements
rather than bytes.

Regression coverage: `tests/automated/check-text-substring-bounds.py`.
