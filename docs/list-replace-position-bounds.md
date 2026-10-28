---
nav_exclude: true
---
# List replacement position bounds

The public List replacement APIs use one-based `int` positions. Replacement
now rejects a position beyond the current item count before converting it to a
zero-based index, and truncates the requested count using subtraction rather
than an overflow-prone index-plus-count expression.

Regression coverage: `tests/automated/check-list-replace-position-bounds.py`.
