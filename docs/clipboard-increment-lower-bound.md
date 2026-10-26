---
nav_exclude: true
---

# Keep clipboard INCR chunks positive

The clipboard transfer chunk calculation now clamps to one byte when an X11
server reports an unusually small maximum request size.  This avoids negative
or zero lengths reaching `XGetWindowProperty` and prevents an append loop from
making no progress.
