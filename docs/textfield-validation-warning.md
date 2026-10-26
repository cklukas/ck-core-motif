---
nav_exclude: true
---
# TextField validation warnings

Unsupported-character diagnostics during TextField validation now tolerate
failure to allocate their temporary escaped string.  The warning is skipped,
but validation continues and no null buffer is formatted or freed.

Regression coverage: `tests/automated/check-textfield-validation-warning.py`.
