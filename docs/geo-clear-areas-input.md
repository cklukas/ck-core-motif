---
nav_exclude: true
---

# Geometry-area clearing input validation

`_XmGeoClearRectObjAreas` now returns safely when the rectangle object, old
geometry record, or parent widget is missing. Valid expose-area clearing is
unchanged.

The focused regression check is
`tests/automated/check-geo-clear-areas-input.py`.
