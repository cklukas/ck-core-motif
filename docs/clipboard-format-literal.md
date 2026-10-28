---
nav_exclude: true
---

# Clipboard format-atom literal formatting

`ClipboardGetAtomFromFormat` now passes its fixed internal format string
directly to `snprintf`, preserving the existing checked allocation while
allowing the compiler to validate the format and argument types.

Regression: `tests/automated/check-clipboard-format-literal.py`
