---
nav_exclude: true
---

# Transfer encoding lookup inputs

`XmeGetEncodingAtom` now rejects a null widget before deriving its application
context or querying the display. Invalid optional handles return `None`; valid
encoding-atom discovery is unchanged.

The focused regression check is
`tests/automated/check-transfer-encoding-inputs.py`.
