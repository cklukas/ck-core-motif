---
nav_exclude: true
---

# Geometry value-loading input validation

`_XmGeoLoadValues` now returns safely when the widget or output record is
missing, or when an instigator has no request record. Valid preferred and
actual geometry loading remains unchanged.

The focused regression check is
`tests/automated/check-geo-load-values-input.py`.
