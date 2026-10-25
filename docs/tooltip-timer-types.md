---
nav_exclude: true
---
# Tooltip timer initialization

The VendorS tooltip trait stores timeout handles as `XtIntervalId`.  Its
initialization used pointer-to-`int` null casts, which produced diagnostics and
could truncate a sentinel on platforms where timeout IDs are wider than an
integer.

The fields now initialize with typed zero timeout IDs, matching the rest of the
tooltip lifecycle and preserving the existing no-timer state.

`tests/automated/check-tooltip-timer-types.py` protects the declaration and
initialization contract.
