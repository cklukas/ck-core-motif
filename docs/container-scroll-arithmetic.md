---
nav_exclude: true
---
# Container scroll arithmetic

Auto-scroll navigator positions now apply increments through a wide,
saturating helper before converting back to `int`. Extreme increments cannot
overflow signed arithmetic before the normal range clamp runs.

The source contract is checked by
`tests/automated/check-container-scroll-arithmetic.py`.
