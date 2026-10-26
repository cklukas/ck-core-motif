# Image-cache scaled buffer bounds

Scaled image rows are allocated through Xt after their byte count is computed
in `size_t`.  The byte count is now also required to fit Xt's `Cardinal`
argument before calling `XtMalloc`, preventing truncation on 64-bit hosts.
Oversized scaled buffers are discarded cleanly; normal scaling is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-cache-buffer-bounds.py
```
