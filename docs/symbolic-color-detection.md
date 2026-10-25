---
nav_exclude: true
---
# XPM symbolic color detection

ImageCache now returns true when `SymbolicColorUsed` finds the requested
symbolic color, while absent inputs and unmatched colors return false. This
restores the cache’s intended color-sensitivity policy for XPM images without
changing ordinary color handling.

The focused regression is `tests/automated/check-symbolic-color-used.py`.
