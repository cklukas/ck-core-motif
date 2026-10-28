---
nav_exclude: true
---

# UIL unknown-entry dump pointer width

The fallback symbol-entry diagnostic now prints its entry pointer with `%p`
and `void *`, while retaining the legacy-width byte value formatting.

Regression: `tests/automated/check-uil-unknown-dump-pointer.py`
