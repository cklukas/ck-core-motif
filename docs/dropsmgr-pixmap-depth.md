---
nav_exclude: true
---
# DropSMgr pixmap depth storage

Drop-site animation pixmap depth is stored as a `Cardinal`, while
`XmeGetPixmapData` writes through an `int *`.  DropSMgr now queries into a
local `int` and assigns the result to the resource field afterward.  This
avoids type-punning through the field address and keeps the public structure
layout unchanged.

The focused regression is `tests/automated/check-dropsmgr-pixmap-depth.py`.
