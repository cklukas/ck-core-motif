---
nav_exclude: true
---

# XPM hash growth failure state

Hash-table growth now publishes the new size, limit, and atom-table pointer
only after replacement allocation succeeds.  An allocation failure therefore
leaves the existing table metadata coherent and retryable.

Regression: `tests/automated/check-xpm-hash-growth-state.py`
