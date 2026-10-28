---
nav_exclude: true
---

# Text source block length bounds

`_XmStringSourceReadString` now checks the gap-prefix byte count before
converting it to the legacy `int` `XmTextBlock` length. An unrepresentable
prefix returns an empty block instead of truncating the pointer difference.

Regression: `tests/automated/check-text-source-block-length.py`
