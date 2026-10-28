---
nav_exclude: true
---

# Clipboard start-retrieve lock status

`XmClipboardStartRetrieve` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening the clipboard
header. This keeps callers informed about display, ownership, and lock
failures instead of continuing with an invalid retrieval transaction.

Regression: `tests/automated/check-clipboard-start-retrieve-lock-status.py`
