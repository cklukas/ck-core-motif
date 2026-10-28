---
nav_exclude: true
---

# Clipboard callback-table allocation width

The callback-table growth path in `XmClipboardStartCopy` now computes
replacement byte sizes in `size_t`, converts the already-bounded sizes
explicitly to Xt’s `Cardinal`, and uses wide byte counts for table copies.

Regression: `tests/automated/check-clipboard-callback-table-width.py`
