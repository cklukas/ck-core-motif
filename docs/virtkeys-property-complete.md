---
nav_exclude: true
---

# Complete virtual-key binding properties

Virtual-key initialization now checks the return status from
`XGetWindowProperty` and rejects `_MOTIF_BINDINGS` or
`_MOTIF_DEFAULT_BINDINGS` data when the server reports remaining bytes.  A
truncated property is discarded and the normal fallback binding selection is
used, avoiding silent parsing of incomplete input.

Regression: `tests/automated/check-virtkeys-property-complete.py`
