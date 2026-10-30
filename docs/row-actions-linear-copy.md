---
nav_exclude: true
---
# RowActions label assembly

`XmRowActions` now combines labels and shortcuts with `XmStringConcat`,
preserving compound-string segments in the displayed action text instead of
flattening the values into a temporary character buffer.

The source contract is checked by:

```sh
python3 tests/automated/check-row-actions-linear-copy.py
```
