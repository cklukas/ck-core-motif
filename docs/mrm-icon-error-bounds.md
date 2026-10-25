---
nav_exclude: true
---
# Mrm icon diagnostic bounds

Icon loading and color realization report filenames, color names, and decoded
color values through a fixed 300-byte diagnostic buffer. Those messages now
use `snprintf`, preserving normal diagnostics while safely truncating unusually
long input instead of overwriting stack storage.

The source-level regression is `tests/automated/check-mrm-icon-error-bounds.py`.
