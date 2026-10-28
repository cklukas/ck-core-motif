---
nav_exclude: true
---

# XPM compressed-file fallback bounds

The z-pipe implementation now includes the POSIX declarations it uses. When
an XPM file is not found and compressed suffixes are tried, the fallback
filename allocation also checks `strlen(filename) + 4` before allocating or
writing the `.Z`/`.gz` suffix.

Regression: `tests/automated/check-xpm-compressed-name-bounds.py`
