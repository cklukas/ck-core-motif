---
nav_exclude: true
---

# Geometry request input validation

`_XmMakeGeometryRequest` now returns `XtGeometryNo` when its widget or
geometry request is missing, rather than passing invalid pointers into Xt.
Normal request and “almost” retry behavior is unchanged.

The focused regression check is
`tests/automated/check-geo-request-input.py`.
