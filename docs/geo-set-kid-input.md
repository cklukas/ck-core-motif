---
nav_exclude: true
---

# Child geometry application input

`_XmSetKidGeo` now treats a null geometry list as a safe no-op before
iterating it. Valid geometry application remains unchanged.

The focused regression check is
`tests/automated/check-geo-set-kid-input.py`.
