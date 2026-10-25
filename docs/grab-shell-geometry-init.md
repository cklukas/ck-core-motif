---
nav_exclude: true
---
# GrabShell geometry initialization

`XmGrabShell` accumulates shadow and border extents into the geometry request
used when its first managed child is absent or does not provide one dimension.
The request structure now starts zeroed, so those dimensions remain defined and
the shell does not depend on an uninitialized stack value. Child-provided
widths, heights, and border widths retain their existing precedence.

The focused regression is `tests/automated/check-grab-shell-geometry-init.py`.
