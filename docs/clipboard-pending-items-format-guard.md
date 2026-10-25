---
nav_exclude: true
---

# Clipboard pending-items format guard

`XmClipboardInquirePendingItems` requires a format name to find matching
formats. It now rejects a null name before locking or enumerating the
clipboard, avoiding an invalid atom lookup while preserving valid pending-item
queries.

The focused regression check is
`tests/automated/check-clipboard-pending-items-format-guard.py`.
