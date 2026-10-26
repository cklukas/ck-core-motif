---
nav_exclude: true
---
# DataField validation lengths

`df_ValidateString` now rejects source strings whose byte or wide-character
length cannot be represented by its legacy integer fields or Xt allocation
width. This prevents truncation before validation and temporary-buffer sizing
for very large values.

Regression coverage: `tests/automated/check-datafield-validate-length.py`.
