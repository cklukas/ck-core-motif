# Image scaling bounds

Image-to-pixmap conversion validates source rectangles, scaled dimensions,
row strides, and intermediate allocation sizes before creating the destination
image or coordinate tables. Invalid or unrepresentable image dimensions are
rejected cleanly, while ordinary scaling continues to use the existing cache
and rendering APIs.

The scaler also rejects a destination origin whose vertical extent cannot be
represented by the signed X11 coordinate type. This keeps the strip upload's
`dest_y + min_y` calculation defined while preserving valid off-screen origins.

The native regression exercises malformed image dimensions and calls the
private scaler with values that would previously overflow its legacy
coordinate and allocation arithmetic:

```sh
python3 tests/automated/run-image-regressions.py /absolute/path/to/build /absolute/path/to/results
```
