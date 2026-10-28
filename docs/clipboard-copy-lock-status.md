---
nav_exclude: true
---

# Clipboard copy lock status

`XmClipboardCopyByName` now treats every non-success result from
`ClipboardLock` as a failed operation.  It releases the retrieved header and
returns the original status, instead of continuing to modify clipboard data
after a lock-allocation or lock-property failure.

Regression: `tests/automated/check-clipboard-copy-lock-status.py`
