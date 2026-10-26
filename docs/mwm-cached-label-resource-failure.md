# MWM cached-label X resource failure

`MakeCachedLabelPixmap()` now checks both `XCreatePixmap()` and `XCreateGC()`.
If either resource cannot be created, it returns `None`; when the pixmap exists
but the GC fails, it frees that pixmap first.  Successful cache lookup and
label-pixmap generation are unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-resource-failure.py
```
