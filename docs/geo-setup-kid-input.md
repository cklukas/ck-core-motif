---
nav_exclude: true
---

# Geometry child setup input validation

`_XmGeoSetupKid` now returns `False` when its destination geometry record is
missing. Valid managed-child setup remains unchanged.

The focused regression check is
`tests/automated/check-geo-setup-kid-input.py`.
