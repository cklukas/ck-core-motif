# ImageCache color reference counts

Cached colors are released by reference count.  A duplicate release or stale
cache entry must not wrap the unsigned counter to its maximum value, because
that would prevent the color from ever being removed and freed.  Releases now
reject an already-zero counter before decrementing it.

`tests/automated/check-image-color-refcount.py` protects this invariant.
