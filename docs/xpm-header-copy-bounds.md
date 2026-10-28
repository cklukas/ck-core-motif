---
nav_exclude: true
---

# XPM header-copy bounds

Array serialization now copies exactly the allocated header-line table and
only writes a successor pointer when another header entry exists.  The final
header pointer is reserved for the pixel section, avoiding an out-of-table
successor write on edge dimensions.

Regression: `tests/automated/check-xpm-header-copy-bounds.py`
