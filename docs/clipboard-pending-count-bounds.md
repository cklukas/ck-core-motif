---
nav_exclude: true
---

# Clipboard pending-item count bounds

Clipboard headers store item counts wider than the legacy `int` loop and Xt
allocation types used by pending-item enumeration.  Header validation now
rejects counts above `INT_MAX`, and the pending-list allocation rejects sizes
above `UINT_MAX / sizeof(XmClipboardPendingRec)` before casting to `Cardinal`.

Regression: `tests/automated/check-clipboard-pending-count-bounds.py`
