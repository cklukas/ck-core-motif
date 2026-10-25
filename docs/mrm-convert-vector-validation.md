---
nav_exclude: true
---
# Mrm conversion vector validation

`Urm__CW_ConvertValue` now validates Char8, compound-string, and integer
vector pointers, validation markers, and counts before repacking them for Xt.
Malformed resource values are rejected cleanly while valid vectors retain the
same pointer-table representation.

The source-level regression is
`tests/automated/check-mrm-convert-vector-validation.py`.
