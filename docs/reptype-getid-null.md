---
nav_exclude: true
---

# `XmRepTypeGetId` null-name validation

`XmRepTypeGetId` now returns `XmREP_TYPE_INVALID` when its type-name argument
is `NULL`, avoiding an invalid `strcmp` call. Normal static and dynamic
representation-type lookup remains unchanged.

The focused regression check is
`tests/automated/check-reptype-getid-null.py`.
