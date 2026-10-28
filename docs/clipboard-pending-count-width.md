---
nav_exclude: true
---

# Clipboard pending-item count width

`XmClipboardInquirePendingItems` now uses unsigned-long loop and result
counters to match `header->currItems` and the public count output. Large,
valid pending-item collections no longer risk signed counter overflow during
the traversal.

Regression: `tests/automated/check-clipboard-pending-count-width.py`
