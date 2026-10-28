---
nav_exclude: true
---

# Clipboard cancellation lock status

`XmClipboardCancelCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before deleting clipboard
records.

Regression: `tests/automated/check-clipboard-cancel-lock-status.py`
