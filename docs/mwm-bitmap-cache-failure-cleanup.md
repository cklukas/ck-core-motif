# MWM failed bitmap-cache loads

`GetBitmapIndex()` reserves a cache slot before loading a named X bitmap.  A
failed read must leave that uncounted slot reusable: retaining its freed path
would leak the next replacement path and risks accidental future use.  A
successful read of a zero-sized bitmap likewise owns an X pixmap that must be
released before rejecting the invalid image.

Both failure paths now clear the slot's path, pixmap, width, and height after
releasing owned resources.  The cache count remains unchanged, so successful
bitmap caching and index ordering are unaffected.

Run the focused source-contract check with:

```
python3 tests/automated/check-mwm-bitmap-cache-failure-cleanup.py
```
