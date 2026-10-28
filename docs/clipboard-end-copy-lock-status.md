---
nav_exclude: true
---

# Clipboard end-copy lock status

`XmClipboardEndCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state and finalizing the copy transaction.

Regression: `tests/automated/check-clipboard-end-copy-lock-status.py`
