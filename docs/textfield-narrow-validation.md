---
nav_exclude: true
---
# TextField narrow-value validation

When wide input is converted to single-byte TextField storage, validation now
checks the destination allocation before calling `wcstombs()`.  Failure clears
the value state, releases temporary storage, and returns safely.

Regression coverage: `tests/automated/check-textfield-narrow-validation.py`.
