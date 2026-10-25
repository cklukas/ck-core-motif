---
nav_exclude: true
---
# Xm image-cache screen validation

`GetImage` needs a valid screen before it can obtain a display. It now rejects
a null `Screen *` together with a null image output before calling
`DisplayOfScreen`, while still clearing valid image outputs on failure.

This prevents `_XmGetImage` callers with an invalid screen from crashing in the
cache entry point. Valid screen and image loads are unchanged. The source-level
regression is `tests/automated/check-xm-get-image-screen.py`.
