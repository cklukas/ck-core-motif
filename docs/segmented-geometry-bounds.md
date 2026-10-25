---
nav_exclude: true
---
# Segmented-control geometry bounds

Segmented-control preferred width now accumulates child widths and spacing in
a wider type and clamps the result to the platform `Dimension` maximum. Large
segment sets no longer wrap into unexpectedly narrow geometry requests;
ordinary layouts are unchanged.

`tests/automated/check-segmented-geometry-bounds.py` protects this calculation.
