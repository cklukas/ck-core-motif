---
nav_exclude: true
---
# TextField selection and drop inputs

TextField selection and drop callbacks now reject malformed transfer metadata,
missing drag target arrays, and allocation failures before dereferencing their
temporary selection or drop records.

Regression coverage: `tests/automated/check-textfield-transfer-inputs.py`.
