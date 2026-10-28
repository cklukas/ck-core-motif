---
nav_exclude: true
---

# Clipboard count inquiry lock status

`XmClipboardInquireCount` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening clipboard
state. Failed inquiries no longer continue as if the clipboard were ready.

Regression: `tests/automated/check-clipboard-inquire-count-lock-status.py`
