# MWM confirmation-window geometry bounds

`XGetGeometry` reports the confirmation window's width and height as unsigned
values, while `ClientData` stores them as signed `int` fields.  The
confirmation path now rejects dimensions above `INT_MAX` before copying them
into the client record and before deriving the min/base/max size fields.

This prevents an oversized or malformed X11 geometry reply from turning into a
negative frame size.  Normal confirmation windows retain their existing
geometry and placement behavior.
