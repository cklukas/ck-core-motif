---
nav_exclude: true
---

# Drag ICC property request length

`_XmGetDragReceiverInfo` now stores its fixed XGetWindowProperty request
length in `long`, matching Xlib's API. Returned byte counts remain unsigned,
so the change does not alter property validation or the wire format.

Regression: `tests/automated/check-dragicc-property-length.py`
