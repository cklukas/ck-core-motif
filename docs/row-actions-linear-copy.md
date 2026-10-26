---
nav_exclude: true
---
# RowActions label assembly

`XmRowActions` now fills its pre-sized label-and-shortcut buffer with tracked
offsets instead of repeated string scans. The displayed action text is
unchanged, while long action rows avoid unnecessary copying.

The source contract is checked by:

```sh
python3 tests/automated/check-row-actions-linear-copy.py
```
