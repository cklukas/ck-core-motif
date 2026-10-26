# MWM stretcher-corner geometry

`StretcherCorner()` now validates its lists and dimensions before generating
resize-handle rectangles.  It requires a two-pixel border, at least a
three-pixel corner in each direction, and a border no wider than either corner
dimension.  Invalid compact geometry is skipped instead of wrapping
expressions such as `cwidth - 3`, `cheight - 3`, or `swidth - 2`.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-stretcher-corner-geometry.py
```
