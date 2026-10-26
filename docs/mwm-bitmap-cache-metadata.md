# MWM bitmap-cache metadata consistency

`GetBitmapIndex()` now returns `-1` when a screen reports populated bitmap
cache entries but has no cache allocation.  This prevents the search loop from
dereferencing inconsistent metadata while preserving normal cache lookup and
growth behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bitmap-cache-metadata.py
```
