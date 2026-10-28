---
nav_exclude: true
---
# Standard conversion WM_CLASS ownership

`XGetWindowProperty` returns `WM_CLASS` bytes owned by Xlib. Standard Motif
conversion now copies valid format-8 data into Xt-owned storage and releases
the Xlib buffer. Malformed formats or allocation failure produce an empty
conversion instead of exposing a buffer with the wrong allocator contract.

Regression coverage: `tests/automated/check-standard-convert-class-ownership.py`.
