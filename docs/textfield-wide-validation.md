---
nav_exclude: true
---
# TextField wide-value validation

Validation of multibyte TextField values now checks the wide-character
allocation before calling `mbstowcs()`.  Allocation failure clears the value
state, releases the temporary validation buffer, and returns without a null
dereference.

Regression coverage: `tests/automated/check-textfield-wide-validation.py`.
