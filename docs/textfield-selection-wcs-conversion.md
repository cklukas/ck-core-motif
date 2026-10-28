---
nav_exclude: true
---

# TextField wide selection conversion

`XmTextFieldGetSelectionWcs` now preserves the native `size_t` result from
`mbstowcs`, rejects conversion failure, and returns exactly the number of wide
characters converted. This avoids signed-result wraparound and prevents
uninitialized trailing characters from being exposed when the selected text
contains a terminator before the requested bound.

Regression coverage:

```text
python3 tests/automated/check-textfield-selection-wcs-conversion.py
```
