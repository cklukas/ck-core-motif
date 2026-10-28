---
nav_exclude: true
---

# Clipboard copy lock status

`XmClipboardCopy` now stops on every non-success result from `ClipboardLock`,
including lock-property or allocation failures.  It returns the original
status before opening or modifying clipboard records.

Regression: `tests/automated/check-clipboard-copy-api-lock-status.py`
