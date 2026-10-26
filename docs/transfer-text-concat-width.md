---
nav_exclude: true
---

# Selection text concatenation width

`_XmTextToLocaleText` now rejects a converted string total that cannot fit
the `Cardinal` size accepted by `XtMalloc`, including the terminator byte.
Normal text conversion is unchanged; oversized external selection data is
reported as a failed conversion instead of being truncated at the allocator
boundary.

The focused regression check is
`tests/automated/check-transfer-text-concat-width.py`.
