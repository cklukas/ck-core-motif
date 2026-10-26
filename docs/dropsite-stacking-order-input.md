---
nav_exclude: true
---

# Drop-site stacking-order input safety

`XmDropSiteQueryStackingOrder` now rejects a null widget or output pointer
before entering the application context. Its child traversal uses `Cardinal`
indices throughout, including a decrement-before-access reverse loop, so
large child counts cannot be narrowed to `int` or underflow the reverse
iteration. Existing stacking order and output ownership are unchanged.

The companion configure/get-visuals APIs now reject null widgets before app
context lookup, use full-width indices for reordering, and return cleanly if
the active-visuals record cannot be allocated.

The focused regression check is
`tests/automated/check-dropsite-stacking-order-input.py`.
