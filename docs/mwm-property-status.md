---
nav_exclude: true
---

# Motif WM property status handling

`XmIsMotifWMRunning` now checks the status returned by
`XGetWindowProperty` before examining the returned property metadata.  An X
error therefore produces the normal `False` result instead of allowing
uninitialized type, format, or length values to influence WM detection.

Regression: `tests/automated/check-mwm-property-status.py`
