# Region-from-image input validation

`_XmRegionFromImage` now returns `NULL` for a NULL `XImage`, missing pixel
data, or non-positive dimensions before calling `XGetPixel`. This keeps
drag-icon and other internal callers from crashing or producing invalid
geometry when image conversion fails upstream.

The focused source-contract check is:

```text
python3 tests/automated/check-region-from-image-input.py
```
