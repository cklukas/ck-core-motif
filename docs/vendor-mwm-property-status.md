---
nav_exclude: true
---

# Vendor-shell WM property status handling

The vendor-shell `_MWM_HINTS` reader now checks the result of
`XGetWindowProperty` before examining the returned type, format, and item
count.  X errors therefore use the configured vendor fallback instead of
depending on uninitialized property metadata.

Regression: `tests/automated/check-vendor-mwm-property-status.py`
