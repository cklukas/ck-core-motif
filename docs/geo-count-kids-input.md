---
nav_exclude: true
---

# Managed-child counting input validation

`_XmGeoCount_kids` now returns zero for a missing composite or child array and
skips null child slots. Valid managed-child counts remain unchanged, while
partially initialized composite state no longer crashes geometry setup.

The focused regression check is
`tests/automated/check-geo-count-kids-input.py`.
