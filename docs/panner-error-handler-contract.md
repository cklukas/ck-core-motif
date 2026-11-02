# Panner Xlib error-handler contract

Panner redraws a live X11 window tree, so windows can disappear between the
tree query and attribute lookup.  The temporary Xlib error handler is now
stored with the exact `XErrorHandler` type, and an attribute record is used
only after `XGetWindowAttributes` reports success.  This preserves the redraw
behavior while avoiding an incompatible callback assignment and stale data.

The focused source-contract check is:

```sh
python3 tests/automated/check-panner-error-handler.py
```
