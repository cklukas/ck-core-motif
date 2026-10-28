---
nav_exclude: true
---

# DragUnder geometry narrowing

Drag-under highlight and shadow regions use X11 `Position`, a signed 16-bit
coordinate type.  Region origins are now offset through a range-checked helper
before narrowing, preventing wraparound when a drop-site region is near the
coordinate limit.  Pixmap strip heights use explicit `Dimension` narrowing
after the existing thickness checks.

Regression: `tests/automated/check-dragunder-geometry.py`
