---
nav_exclude: true
---

# Clipboard copy allocation width

`XmClipboardCopy` now rejects format-record and format-data lengths that do
not fit Xt’s `Cardinal` allocator size, and converts bounded lengths
explicitly before allocation.

Regression: `tests/automated/check-clipboard-copy-allocation-width.py`
