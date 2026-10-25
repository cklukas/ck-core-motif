---
nav_exclude: true
---
# Mrm pointer-list resize ownership

`UrmPlistResize` now stores the `XtRealloc` result in a temporary pointer and
updates the list only after allocation succeeds. A failed resize therefore
leaves the existing pointer vector and slot count intact for the caller to
handle.
