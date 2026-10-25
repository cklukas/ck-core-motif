---
nav_exclude: true
---

# Geometry comparison input validation

`_XmGeometryEqual` now returns `False` when the widget or either geometry
record is missing. Valid geometry comparisons retain their existing behavior,
while incomplete requests no longer reach request-mask macros with null
pointers.

The focused regression check is
`tests/automated/check-geo-equal-inputs.py`.
