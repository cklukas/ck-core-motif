---
nav_exclude: true
---

# Clipboard format inquiry buffer cleanup

`XmClipboardInquireFormat` now releases the external `TARGETS` buffer whether
the requested 1-based format exists or the owner returned too few targets.
The latter remains a normal no-data result without leaking the conversion
buffer.

Regression: `tests/automated/check-clipboard-inquire-format-buffer.py`
