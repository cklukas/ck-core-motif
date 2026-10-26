# MWM bitmap-cache allocation retry

`GetBitmapIndex()` now resets `bitmapCacheSize` when the cache is empty but its
allocation is null.  This is the state left by a failed initial allocation;
resetting it lets the next lookup enter the normal creation path and retry
instead of returning `-1` forever.

The populated-cache consistency guard remains in place: a nonzero count with a
null pointer is still rejected.  Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bitmap-cache-retry.py
```
