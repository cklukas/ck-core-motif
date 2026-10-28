---
nav_exclude: true
---

# Clipboard length inquiry lock status

`XmClipboardInquireLength` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. A failed length inquiry no longer proceeds with an invalid lock.

Regression: `tests/automated/check-clipboard-inquire-length-lock-status.py`
