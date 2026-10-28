---
nav_exclude: true
---

# Clipboard undo-copy lock status

`XmClipboardUndoCopy` now stops on every non-success result from
`ClipboardLock`, preserving the original status before opening the clipboard
header. A failed lock can no longer be treated as a usable clipboard state.

Regression: `tests/automated/check-clipboard-undo-lock-status.py`
