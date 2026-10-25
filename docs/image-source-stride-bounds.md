---
nav_exclude: true
---
# Scaled image source bounds

Scaled image conversion reads source pixels directly for the fast 8-bit path
and through Xlib helpers for other formats.  It now rejects an image with no
pixel data and rejects a height/row-stride product that cannot fit in
`size_t` before any row offset is formed.  Malformed images therefore fail
cleanly instead of dereferencing null data or wrapping a row address.

`tests/automated/check-image-source-stride-bounds.py` protects these checks.
