---
nav_exclude: true
---

# Motif WM property status handling

`XmIsMotifWMRunning` now checks the status returned by
`XGetWindowProperty` before examining the returned property metadata.  An X
error therefore produces the normal `False` result instead of allowing
uninitialized type, format, or length values to influence WM detection.

The query also requires a complete non-null property value, and initializes the
optional `XQueryTree` child-list result before cleanup. Truncated properties and
failed tree queries therefore produce the normal `False` result without
dereferencing or freeing invalid storage.

Regression: `tests/automated/check-mwm-property-status.py`
