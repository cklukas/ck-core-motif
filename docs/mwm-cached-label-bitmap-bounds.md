# MWM cached-label bitmap bounds

`MakeCachedLabelPixmap()` now validates its screen record, menu widget, and
bitmap index before accessing the cache.  Negative or out-of-range indices,
an empty cache, and missing inputs return `None`; valid cached labels retain
their existing color lookup and pixmap reuse behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-bitmap-bounds.py
```
