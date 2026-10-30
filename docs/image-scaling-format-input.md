# Image-scaling format input

`_XmPutScaledImage` now accepts only the three X11-defined `XImage` formats:
`XYBitmap`, `XYPixmap`, and `ZPixmap`.  Unknown format values are rejected
before the scaler selects its byte-copy or `XGetPixel` path, keeping malformed
images from reaching format-dependent Xlib operations.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-scaling-format-input.py
```
