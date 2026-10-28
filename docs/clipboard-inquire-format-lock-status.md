---
nav_exclude: true
---

# Clipboard format inquiry lock status

`XmClipboardInquireFormat` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. A failed format inquiry no longer proceeds with an invalid lock.

Regression: `tests/automated/check-clipboard-inquire-format-lock-status.py`
