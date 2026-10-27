---
nav_exclude: true
---
# File-selection drop format validation

File-selection text drops now reject external 16-bit and 32-bit formats, and
lengths that cannot be represented by the text conversion path, before locale
conversion or direct insertion.

Regression coverage: `tests/automated/check-textfsel-drop-format.py`.
