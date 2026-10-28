---
nav_exclude: true
---

# Clipboard end-copy offset bounds

`XmClipboardEndCopy` now rejects invalid item IDs and checks the item-list
byte count, offset addition, and final conversion before forming the new item
pointer. This keeps malformed or overflowing clipboard headers from turning
signed arithmetic into an out-of-range write.

Regression: `tests/automated/check-clipboard-end-copy-offset.py`
