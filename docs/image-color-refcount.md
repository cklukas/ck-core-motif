---
nav_exclude: true
---
# ImageCache color reference counts

Cached colors are released by reference count.  A duplicate release or stale
cache entry must not wrap the unsigned counter to its maximum value, because
that would prevent the color from ever being removed and freed.  Releases now
reject an already-zero counter before decrementing it.

Lookups also refuse to increment a counter that has reached `UINT_MAX`.
This avoids wrapping a live reference count to zero, which could otherwise
allow a still-referenced color to be released. The same focused regression
checks both RGB and symbolic-name lookup paths.

`tests/automated/check-image-color-refcount.py` protects this invariant.
