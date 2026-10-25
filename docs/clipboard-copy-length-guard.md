---
nav_exclude: true
---

# Clipboard copy length truncation guard

The legacy clipboard retrieval helper accepts its extension length as an
`int`, while the public copy APIs accept `unsigned long`. `XmClipboardCopy`
and `XmClipboardCopyByName` now reject lengths above `INT_MAX` before the
conversion, preventing a wrapped length from producing an undersized property
buffer. Normal clipboard copies are unchanged.

The focused regression check is
`tests/automated/check-clipboard-copy-length-guard.py`.
