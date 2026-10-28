---
nav_exclude: true
---

# Complete tear-off WM hints

Tear-off creation now checks the X property request status and requires all
five `_MWM_HINTS` items before copying the property into the outgoing hints
structure.  A short or failed property is discarded instead of silently
replacing valid window-manager function and decoration fields with zeros.

Regression: `tests/automated/check-tearoff-hints-property.py`
