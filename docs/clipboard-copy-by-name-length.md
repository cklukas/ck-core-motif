---
nav_exclude: true
---

# Clipboard copy-by-name length bounds

`XmClipboardCopyByName` now applies the same signed-range checks as the
ordinary copy path when updating a format record. Negative serialized lengths
and additions that exceed `LONG_MAX` fail cleanly and release the active copy
buffers before returning.

Regression: `tests/automated/check-clipboard-copy-by-name-length.py`
