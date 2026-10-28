---
nav_exclude: true
---

# Clipboard replacement request size

`ClipboardReplaceItem` now computes its incremental request size in unsigned
wide arithmetic and caps it at `INT_MAX`, matching `XChangeProperty`’s element
count parameter before each chunk is submitted.

Regression: `tests/automated/check-clipboard-replace-request-size.py`
