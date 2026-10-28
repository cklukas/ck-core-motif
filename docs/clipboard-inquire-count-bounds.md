---
nav_exclude: true
---

# Clipboard inquiry count bounds

`XmClipboardInquireCount` now validates external `TARGETS` data before
converting its byte length to the legacy `int` format count.  Non-atom-aligned
or unrepresentably large responses are rejected after releasing the owner
buffer and clipboard lock.

Regression: `tests/automated/check-clipboard-inquire-count-bounds.py`
