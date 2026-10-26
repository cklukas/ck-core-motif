---
nav_exclude: true
---
# XmTextSetStringWcs conversion termination

`XmTextSetStringWcs` now explicitly terminates successful multibyte
conversions.  A conversion that fills the complete destination capacity is
treated as non-terminated and replaced with an empty string rather than being
passed to string consumers without a NUL terminator.

Regression coverage: `tests/automated/check-text-set-string-wcs-termination.py`.
