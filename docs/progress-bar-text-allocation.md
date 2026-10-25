---
nav_exclude: true
---

# Progress-bar text allocation guard

`XmProgressBar` now checks the optional percentage `XmString` before measuring,
drawing, or freeing it. If allocation fails, the bar remains rendered and only
the percentage text is omitted for that redisplay.

The focused regression check is
`tests/automated/check-progress-bar-text-allocation.py`.
