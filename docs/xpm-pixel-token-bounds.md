---
nav_exclude: true
---

# XPM pixel-token bounds

XPM file output now rejects zero `cpp` and color tokens shorter than the
declared characters-per-pixel width. Valid tokens are copied exactly, avoiding
uninitialized padding from `strncpy`.

Regression: `tests/automated/check-xpm-pixel-token-bounds.py`
