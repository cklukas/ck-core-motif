---
nav_exclude: true
---

# Clipboard paste lock cleanup

`XmClipboardCopyByName` retrieves a temporary clipboard header before acquiring
the clipboard lock.  When another client owns that lock, the function now
releases the retrieved header before returning `ClipboardLocked`; successful
and failure paths retain their existing protocol behavior.

Regression: `tests/automated/check-clipboard-paste-lock-cleanup.py`
