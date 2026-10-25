---
nav_exclude: true
---

# Progress-ring geometry bounds

`XmProgressRing` now clamps default and queried width/height calculations after
combining the ring diameter, label extent, and spacing. Normal layouts are
unchanged; large labels or spacing values no longer wrap geometry requests.

The focused regression check is
`tests/automated/check-progress-ring-geometry-bounds.py`.
