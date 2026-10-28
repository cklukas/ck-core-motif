---
nav_exclude: true
---
# Safe atom-name fallback

Transfer diagnostics must continue safely when an invalid X selection atom is
reported and the fallback allocation cannot be satisfied. The atom-name helper
now returns a non-owned empty string in that case, and its cleanup path leaves
that sentinel untouched.

Regression coverage: `tests/automated/check-safe-atom-name-fallback.py`.
