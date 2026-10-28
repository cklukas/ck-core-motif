---
nav_exclude: true
---

# Text byte-conversion widths

`_XmTextBytesToCharacters` now passes validated positive lengths explicitly to
size-typed C conversion APIs and checks the `mbstowcs` result before narrowing
it to the public `int` return value. Invalid multibyte input retains the
existing `-1` result.

Regression: `tests/automated/check-text-byte-conversion-width.py`
