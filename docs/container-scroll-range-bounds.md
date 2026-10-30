---
nav_exclude: true
---
# Container scroll range bounds

Container auto-scroll now clamps each navigator axis to its minimum when the
reported range is invalid or the slider already covers the entire range. This
avoids subtracting a larger slider from the maximum and sending a wrapped
position to the navigator.

The source contract is checked by
`tests/automated/check-container-scroll-range-bounds.py`.
