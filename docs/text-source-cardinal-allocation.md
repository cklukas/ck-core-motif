---
nav_exclude: true
---

# Text source Cardinal allocation bounds

`ReadSource` now rejects a block whose byte count cannot fit Xt's `Cardinal`
allocation size, after checking host-size multiplication. The validated size
is passed explicitly to `XtMalloc` and `XtRealloc`, preventing truncation on
platforms where `size_t` is wider than Xt allocation lengths.

Regression: `tests/automated/check-text-source-cardinal-allocation.py`
