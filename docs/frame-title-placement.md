---
nav_exclude: true
---
# Frame title placement bounds

Frame title placement now computes beginning, center, and end horizontal
coordinates in signed arithmetic before converting to the X toolkit
`Position`.  A title wider than a collapsed frame therefore remains a valid
off-edge position instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-frame-title-placement.py`.
