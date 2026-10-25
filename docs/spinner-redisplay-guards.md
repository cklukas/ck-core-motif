---
nav_exclude: true
---
# Spinner redisplay guards

`XmSpinner` now handles partial ring/text GC allocation and skips rendering
when no usable GC exists. Its spinner diameter calculation uses signed
intermediate geometry, avoiding unsigned underflow when spacing exceeds the
widget height, and focus rectangles are omitted when they cannot fit.

The focused source contract is checked by
`tests/automated/check-spinner-redisplay-guards.py`.
