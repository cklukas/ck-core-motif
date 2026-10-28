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
Nonpositive depth values are rejected before plane traversal and mask
calculation as well.
Depths above 32 are rejected because the optimized reader’s low-bit mask table
is defined only for the 32-bit range.
Negative x-offsets are rejected before bit and byte indexing can underflow.
When a shape image is provided, its dimensions must match the source image so
the shared scan geometry cannot overrun either image.

Regression: `tests/automated/check-xpm-scan-entry.py`
