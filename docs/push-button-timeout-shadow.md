---
nav_exclude: true
---
# PushButton timeout shadow bounds

The PushButton menu arm timeout can redraw after a menu entry has been resized
or partially initialized.  Its shadow redraw now requires positive interior
geometry and both shadow GCs, with signed conversion before subtracting the
highlight frame.

Normal timeout feedback keeps its existing shadow type and focus checks.  The
focused regression is
`tests/automated/check-push-button-timeout-shadow.py`.
