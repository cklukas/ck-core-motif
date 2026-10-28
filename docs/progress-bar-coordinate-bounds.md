---
nav_exclude: true
---

# ProgressBar coordinate bounds

ProgressBar keeps its bar origin in signed `int` arithmetic and clamps label
and percentage text coordinates before converting them to Motif's signed
`Position` type.  Very large spacing or font metrics are therefore clipped
safely instead of wrapping to an unrelated screen coordinate.

`tests/automated/check-progress-bar-coordinate-bounds.py` protects this
coordinate contract.
