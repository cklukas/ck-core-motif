---
nav_exclude: true
---

# Progress-ring text allocation guard

`XmProgressRing` now checks the optional percentage `XmString` before measuring
or drawing it. If allocation fails, the ring and track remain rendered and only
the percentage text is omitted for that redisplay.

The focused regression check is
`tests/automated/check-progress-ring-text-allocation.py`.
