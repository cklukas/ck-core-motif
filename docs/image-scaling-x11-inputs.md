---
nav_exclude: true
---
# Scaled-image X11 input validation

`_XmPutScaledImage` now returns before entering Xlib when its display, target
drawable, or graphics context is missing. Existing image, dimension, and
allocation checks remain unchanged; valid scaling calls keep their behavior.

`tests/automated/check-image-scaling-x11-inputs.py` protects this contract.
