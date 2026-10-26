---
nav_exclude: true
---
# Drag-under composite masks

`DrawPixmap` now validates the temporary composite mask and its graphics
context before using either resource. If allocation fails, the temporary mask
is released and drawing falls back to the ordinary clip region, avoiding X
requests against an invalid drawable while preserving the animation.

The focused source contract is checked by
`tests/automated/check-dragunder-mask.py`.
