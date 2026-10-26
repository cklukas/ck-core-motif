# MWM confirmation-window border width

The confirmation-window path in `GetWmClientInfo` receives the X11 border
width as an `unsigned int`, but the client record stores it as an `int`.
Passing the record field directly to `XGetGeometry` through a cast could write
an out-of-range value and leave a negative border width in later frame
geometry.  The path now uses a correctly typed temporary, rejects values that
cannot be represented by the client record, and releases the unregistered
client record on that failure.

Normal border widths are copied unchanged, so existing confirmation-window
layout remains compatible.
