---
nav_exclude: true
---

# List horizontal-origin bounds

List now clamps the `MaxWidth - listwidth` scroll-origin calculation before
storing it in the signed `Position` XOrigin field.  Very wide content cannot
wrap the horizontal origin and shift every row to an unrelated coordinate.

`tests/automated/check-list-horizontal-origin-bounds.py` protects this
scrolling contract.
