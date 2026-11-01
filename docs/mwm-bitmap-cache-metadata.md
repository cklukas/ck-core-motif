# MWM bitmap-cache metadata consistency

`GetBitmapIndex()` now returns `-1` when a screen reports populated bitmap
cache entries but has no cache allocation, or when its entry count exceeds its
recorded allocation capacity. This prevents the search and insertion paths from
dereferencing inconsistent cache metadata while preserving normal cache lookup
and growth behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bitmap-cache-metadata.py
```
