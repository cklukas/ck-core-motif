---
nav_exclude: true
---

# XPM automatic chars-per-pixel growth

When an XPM image does not provide enough characters per pixel for its color
count, the scanner derives the minimum width by repeatedly multiplying the
printable-character capacity.  That multiplication is now checked before it
can wrap an unsigned counter.  An unrepresentable color count returns
`XpmNoMemory` through the existing cleanup path instead of looping with a
corrupted capacity.

Regression: `tests/automated/check-xpm-cpp-growth.py`
