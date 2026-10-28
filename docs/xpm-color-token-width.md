---
nav_exclude: true
---

# XPM color-token width

XPM color-table serialization now receives the declared `cpp` width and
rejects primary color tokens shorter than that width before writing them.

Regression: `tests/automated/check-xpm-color-token-width.py`
