---
nav_exclude: true
---

# Chip highlight bounds

The Chip highlight frame is now drawn only when both dimensions exceed twice
the highlight thickness.  Tiny or explicitly constrained chips no longer wrap
the unsigned frame dimensions passed to Xlib.

`tests/automated/check-chip-highlight-bounds.py` protects this geometry guard.
