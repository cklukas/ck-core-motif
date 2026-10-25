---
nav_exclude: true
---

# Geometry reply comparison input validation

`_XmGeoReplyYes` now returns `False` when either the desired or response
geometry record is missing. Valid geometry replies retain their existing
comparison behavior.

The focused regression check is
`tests/automated/check-geo-reply-yes-inputs.py`.
