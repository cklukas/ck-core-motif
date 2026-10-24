# Image scaling bounds

Image-to-pixmap conversion validates source rectangles, scaled dimensions,
row strides, and intermediate allocation sizes before creating the destination
image or coordinate tables. Invalid or unrepresentable image dimensions are
rejected cleanly, while ordinary scaling continues to use the existing cache
and rendering APIs.

The native regression exercises malformed image dimensions and calls the
private scaler with values that would previously overflow its legacy
coordinate and allocation arithmetic:

```sh
python3 tests/automated/run-image-regressions.py /absolute/path/to/build /absolute/path/to/results
```
