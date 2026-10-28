---
nav_exclude: true
---

# Clipboard item-length bounds

`XmClipboardCopy` now checks the converted format-unit increment against the
existing signed `itemLength` before updating the serialized format record.
Overflow leaves the clipboard transaction cleaned up and returns failure.

Regression: `tests/automated/check-clipboard-item-length-bounds.py`
