---
nav_exclude: true
---

# Clipboard start-copy lock status

`XmClipboardStartCopy` now stops on every non-success result from
`ClipboardLock`, including lock-property or allocation failures.  It returns
the original status instead of opening or modifying the clipboard after lock
acquisition failed.

Regression: `tests/automated/check-clipboard-start-copy-lock-status.py`
