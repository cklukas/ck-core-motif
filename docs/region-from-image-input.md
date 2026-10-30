# Region-from-image input guard

`_XmRegionFromImage` now returns `NULL` for a NULL `XImage` before accessing
its dimensions. This keeps drag-icon and other internal callers from crashing
when image conversion fails upstream.

The focused source-contract check is:

```text
python3 tests/automated/check-region-from-image-input.py
```
