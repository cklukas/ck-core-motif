---
nav_exclude: true
---
# Text source value conversion

`_XmStringSourceGetValue` now uses the checked range-size helper for both byte
and wide-character result buffers.  Allocation failures return `NULL` before
copying into the result, including the empty wide-string case.  The wide
conversion also passes the source block length to `mbstowcs` as a `size_t`,
without truncating a text-position difference to `int`, and rejects a
non-monotonic source callback safely.

`tests/automated/check-text-source-value-allocation.py` protects these checks.
