---
nav_exclude: true
---
# Conversion completion inputs

`_XmConvertComplete` now handles a null callback structure by releasing the
returned conversion value and returning safely.  The destination callback
lookup helper likewise returns `NULL` for a missing transfer ID.  These guards
keep cleanup and callback-destruction paths safe without changing ownership
for valid conversions.

The focused regression is
`tests/automated/check-convert-complete-inputs.py`.
