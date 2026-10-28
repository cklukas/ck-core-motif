---
nav_exclude: true
---

# Clipboard format inquiry indexes

`XmClipboardInquireFormat` uses a 1-based format index.  It now rejects zero
and negative indexes before consulting an external selection owner, preventing
pointer arithmetic before the returned `TARGETS` array while preserving the
existing public indexing convention.

Regression: `tests/automated/check-clipboard-inquire-format-index.py`
