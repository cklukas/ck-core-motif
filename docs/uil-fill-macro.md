---
nav_exclude: true
---

# UIL listing fill helper

The private `_fill` helper now uses a uniquely named `size_t` counter and a
zero-based decrement loop. This avoids colliding with callers' local `i`
variables and keeps fixed-buffer fill lengths unsigned without changing the
generated listing layout.

Regression: `tests/automated/check-uil-fill-macro.py`
