---
nav_exclude: true
---

# Clipboard start-copy allocation width

`XmClipboardStartCopy` now passes its fixed item-record allocation through an
explicit `Cardinal` conversion for Xt, matching the allocator’s API width and
avoiding an implicit narrowing warning.

Regression: `tests/automated/check-clipboard-start-copy-allocation-width.py`
