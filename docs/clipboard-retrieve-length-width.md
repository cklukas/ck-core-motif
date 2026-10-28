---
nav_exclude: true
---

# Clipboard retrieval length width

`ClipboardRetrieveItem` now accepts unsigned-long additional lengths, so
large format-data requests are not truncated through an `int` cast. Before
calling Xt’s `Cardinal` allocator, the combined length is checked explicitly
and rejected if it cannot be represented.

Regression: `tests/automated/check-clipboard-retrieve-length-width.py`
