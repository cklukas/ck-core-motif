---
nav_exclude: true
---

# Clipboard format-list offset validation

Clipboard data-item readers now require `formatIdList` to point after the
fixed `ClipboardDataItemRec`, as well as within the returned record length.
Malformed items cannot make format-list traversal read header bytes or memory
before the record.

Regression: `tests/automated/check-clipboard-format-list-offset.py`
