---
nav_exclude: true
---

# CascadeButton pixmap metadata guard

CascadeButton drawing now checks whether `XmeGetPixmapData` succeeded before
using the returned depth.  Unavailable or invalid pixmap metadata now skips
drawing safely; valid pixmap rendering is unchanged.

The static regression check is
`tests/automated/check-cascade-pixmap-metadata.py`.
