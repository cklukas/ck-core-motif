# Scaled-image destination bounds

`_XmPutScaledImage` now rejects a destination whose X origin plus width would
exceed the signed X11 coordinate range. The Y path already applied the same
protection before constructing strips; the X check prevents a large or
malformed request from wrapping when handed to Xlib. Normal coordinates and
negative origins that remain within the signed range are unchanged.

Regression coverage: `tests/automated/check-image-scaling-destination-bounds.py`.
