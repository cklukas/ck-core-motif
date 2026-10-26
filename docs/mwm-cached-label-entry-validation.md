# MWM cached-label entry validation

`MakeCachedLabelPixmap()` now returns `None` for cache entries with a missing
source bitmap or zero width/height.  This prevents invalid X requests while
preserving normal color-specific cache reuse and label-pixmap creation.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-entry-validation.py
```
