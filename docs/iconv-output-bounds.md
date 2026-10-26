---
nav_exclude: true
---
# Iconv output-buffer growth bounds

The compound-text iconv helper starts with an Xt-sized output buffer and grows
it when conversion reports `E2BIG`.  Growth now rejects a doubling that would
exceed either `size_t` or Xt's `Cardinal` range, preventing a truncated
`XtRealloc` request while preserving normal conversion and retry behavior.

`tests/automated/check-iconv-output-bounds.py` protects the boundary.
