---
nav_exclude: true
---
# TextField wide-value warnings

Unsupported-wide-character diagnostics now tolerate failure to allocate their
temporary escaped strings.  Warning formatting is skipped when necessary while
the validation loop continues without null-buffer access.

Regression coverage: `tests/automated/check-textfield-wide-warning.py`.
