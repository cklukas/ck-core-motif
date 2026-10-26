# MWM icon X resource failure

`MakeIconPixmap()` now checks `XCreatePixmap()` and `XCreateGC()` before using
the resources.  If pixmap creation fails it returns `None`; if GC creation
fails it frees the pixmap and returns `None`.  Successful icon scaling,
masking, and beveling remain unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-resource-failure.py
```
