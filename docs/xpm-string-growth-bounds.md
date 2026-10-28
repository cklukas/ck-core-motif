---
nav_exclude: true
---

# XPM string growth bounds

File-backed XPM string parsing now checks accumulated record lengths before
growing its temporary buffer. An oversized record returns `XpmNoMemory`
instead of allowing `unsigned int` length arithmetic to wrap.

Regression: `tests/automated/check-xpm-string-growth-bounds.py`
