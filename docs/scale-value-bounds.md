---
nav_exclude: true
---
# XmScale value formatting bounds

`XmScale` formats its displayed value and clipboard text using a resource
controlled decimal precision.  The old formatter used `sprintf` and was
called with both 256-byte and 100-byte buffers, so unusually large precision
could overwrite either caller.

`GetValueString` now receives the destination size, uses `snprintf`, checks
room for the locale decimal separator, and leaves a safely terminated result
when the requested precision cannot fit.  Normal values and locale-specific
decimal points retain their existing representation.

`tests/automated/check-scale-value-bounds.py` protects both call sites.
