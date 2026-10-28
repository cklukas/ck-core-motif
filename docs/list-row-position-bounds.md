---
nav_exclude: true
---

# List row-position bounds

List row-height accumulation now uses signed `long long` arithmetic and clamps
the resulting redraw positions before storing them in `Position` variables.
Large item counts, row heights, or spacing values therefore cannot wrap a row
to an unrelated coordinate.

`tests/automated/check-list-row-position-bounds.py` protects this contract.
