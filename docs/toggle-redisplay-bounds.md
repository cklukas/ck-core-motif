---
nav_exclude: true
---
# ToggleButton redisplay bounds

ToggleButton and ToggleButtonGadget redisplay now convert their unsigned widget
dimensions to `int` before subtracting margins and offsets.  The existing
non-negative clamp therefore handles collapsed widgets instead of receiving a
wrapped large value for the clear rectangle.

The focused regression is `tests/automated/check-toggle-redisplay-bounds.py`.
