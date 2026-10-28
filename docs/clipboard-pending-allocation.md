---
nav_exclude: true
---

# Clipboard pending-item allocation

`XmClipboardInquirePendingItems` checks the pending-record count against
`UINT_MAX / sizeof(XmClipboardPendingRec)` before allocating. The validated
byte count is now passed to Xt with an explicit `Cardinal` conversion, making
the allocation contract clear on both 32-bit and 64-bit builds.

Regression: `tests/automated/check-clipboard-pending-allocation.py`
