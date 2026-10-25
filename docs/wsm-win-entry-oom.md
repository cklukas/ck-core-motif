---
nav_exclude: true
---

# WSM window-entry allocation

WSM window-list replies contain two variable-length arrays: window IDs and
matching properties. `UnpackWinEntry` now checks both element counts against
Xt's `Cardinal` allocation width before multiplying and writing. If either
allocation fails, it consumes the corresponding encoded values without storing
them and reports an empty list, preserving stream alignment for subsequent
entries and avoiding null-pointer writes.
