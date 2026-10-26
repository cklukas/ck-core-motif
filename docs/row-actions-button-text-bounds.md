---
nav_exclude: true
---

# Row-actions button-text allocation bounds

`XmRowActions` assembles an action label and optional shortcut into one
temporary buffer. The length is calculated as `size_t`, but `XtMalloc`
accepts an Xt `Cardinal`-sized request. The assembly now rejects a length
that cannot be represented by `Cardinal` before calling `XtMalloc`, avoiding
truncation on platforms where `size_t` is wider than Xt's allocation type.

The focused regression check is
`tests/automated/check-row-actions-button-text-bounds.py`.
