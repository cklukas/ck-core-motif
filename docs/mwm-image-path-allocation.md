---
nav_exclude: true
---
# MWM bitmap path allocation

The window manager combines the configured bitmap directory with a relative
icon name before asking Motif to load the image.  The measured path length can
be unrepresentable on a narrow `size_t`, and the allocator can also fail.  In
either case the lookup now returns the existing unspecified-pixmap result
before formatting into the buffer.  Successful lookup order and path names
are unchanged.

`tests/automated/check-mwm-image-path-allocation.py` protects this contract.
