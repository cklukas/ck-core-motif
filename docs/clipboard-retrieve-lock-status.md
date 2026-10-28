---
nav_exclude: true
---

# Clipboard retrieval lock status

The internal transaction behind `XmClipboardRetrieve` now stops on every
non-success result from `ClipboardLock`, preserving the original status before
opening clipboard state or processing a requested format.

Regression: `tests/automated/check-clipboard-retrieve-lock-status.py`
