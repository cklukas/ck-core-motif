---
nav_exclude: true
---
# Avatar resource transactions

`XmAvatar` now preserves its current initials and render table when a
`SetValues` replacement cannot be copied. Successful replacements remain
owned by the widget; failed replacements do not leave redisplay or geometry
using a null resource.

The focused source check is
`tests/automated/check-avatar-initials-geometry-bounds.py`.
