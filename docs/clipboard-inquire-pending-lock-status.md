---
nav_exclude: true
---

# Clipboard pending-item inquiry lock status

`XmClipboardInquirePendingItems` now stops on every non-success result from
`ClipboardLock`, preserving the original status before reading or allocating
pending-item state.

Regression: `tests/automated/check-clipboard-inquire-pending-lock-status.py`
