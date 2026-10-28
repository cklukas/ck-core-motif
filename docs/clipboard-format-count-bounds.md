---
nav_exclude: true
---

# Clipboard format-count bounds

`ClipboardFindFormat` now rejects impossible cancelled-format counts and
saturates the internal wide count at `INT_MAX` before returning through the
legacy `int *count` API. Malformed or very large records no longer wrap the
count into a negative value.

Regression: `tests/automated/check-clipboard-format-count-bounds.py`
