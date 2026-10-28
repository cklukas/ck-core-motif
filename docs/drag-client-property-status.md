---
nav_exclude: true
---

# Drag client-window property status

The drag client-window probe now checks the return status from
`XGetWindowProperty` before treating a window as a client-window owner.  X
errors continue through the existing child-window search instead of using
undefined property metadata.

Regression: `tests/automated/check-drag-client-property-status.py`
