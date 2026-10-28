---
nav_exclude: true
---

# XPM image-scan entry validation

`XpmCreateXpmImageFromImage` now rejects null display/output arguments and a
call with neither a source image nor a shape image.  This keeps direct callers
on the documented `XpmFileInvalid` path before any Xlib geometry, zero-sized
allocation, or scan arithmetic begins.

Regression: `tests/automated/check-xpm-scan-entry.py`
