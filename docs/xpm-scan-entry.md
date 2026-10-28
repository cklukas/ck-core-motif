---
nav_exclude: true
---

# XPM image-scan entry validation

`XpmCreateXpmImageFromImage` now rejects null display/output arguments and a
call with neither a source image nor a shape image.  This keeps direct callers
on the documented `XpmFileInvalid` path before any Xlib geometry, zero-sized
allocation, or scan arithmetic begins.
It also rejects source `XImage` objects without pixel data, preventing the
reader callbacks from dereferencing a missing buffer.
Nonpositive row strides are rejected as well, so row and plane offsets cannot
underflow in the readers.
Nonpositive bitmap-unit and bits-per-pixel fields are rejected too, preventing
modulo or shift operations from using invalid image geometry.

Regression: `tests/automated/check-xpm-scan-entry.py`
