# MWM stretcher geometry validation

`CreateStretcherWindows()` now checks the `GetFramePartInfo()` result for every
resize handle.  If a frame part is unavailable, that handle is skipped rather
than using uninitialized local coordinates or dimensions in `XCreateWindow()`.
Valid frame records continue to create all available resize handles exactly as
before.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-stretcher-geometry.py
```
