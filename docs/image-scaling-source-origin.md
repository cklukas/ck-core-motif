# Cropped image scaling source origin

`_XmPutScaledImage` now normalizes scaled X coordinates relative to `src_x`,
just as its Y path normalizes coordinates relative to `src_y`. Previously a
nonzero source X origin left the output positioned using full-image coordinates,
which could place pixels outside the destination image. The normalized right
edge is checked against `dest_width` before any pixels are written. Calls with
the usual zero origin retain identical coordinates.

Regression coverage: `tests/automated/check-image-scaling-source-origin.py`.
