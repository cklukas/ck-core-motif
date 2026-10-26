---
nav_exclude: true
---
# Multibyte XmString allocation bounds

`_XmGetMBStringFromXmString` accumulates component text in a wide length type,
then allocates through Xt.  Both conversion paths now reject a final length
whose terminator would exceed Xt’s `Cardinal` range, preventing size truncation
while preserving ordinary multibyte conversion results.

`tests/automated/check-mb-string-cardinal-bounds.py` protects both paths.
