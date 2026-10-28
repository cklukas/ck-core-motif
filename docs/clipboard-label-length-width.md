---
nav_exclude: true
---

# Clipboard compound-string label length width

`XmClipboardStartCopy` now keeps the compound-string byte count in an
unsigned type matching Motif conversion output and explicitly passes the
already-validated label ID through the signed `itemId` API.

Regression: `tests/automated/check-clipboard-label-length-width.py`
