---
nav_exclude: true
---

# Clipboard withdrawal lock status

`XmClipboardWithdrawFormat` now stops on every non-success result from
`ClipboardLock`, preserving the original status before deleting a format.

Regression: `tests/automated/check-clipboard-withdraw-lock-status.py`
