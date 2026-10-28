---
nav_exclude: true
---

# Spinner coordinate bounds

Spinner keeps its ring center in signed arithmetic for X drawing and clamps
the label origin before converting it to Xt's signed `Position` type.  Large
spacing or DPI-derived dimensions therefore clip safely instead of wrapping
the spinner to an unrelated coordinate.

`tests/automated/check-spinner-coordinate-bounds.py` protects this contract.
