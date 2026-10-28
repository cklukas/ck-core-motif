---
nav_exclude: true
---

# Clipboard length bounds

`XmClipboardInquireLength` now rejects a negative serialized format length and
checks the 32-bit format-unit multiplication before producing the public
unsigned length. Corrupt or overflowing records therefore fail cleanly rather
than becoming a large wrapped length.

Regression: `tests/automated/check-clipboard-length-bounds.py`
