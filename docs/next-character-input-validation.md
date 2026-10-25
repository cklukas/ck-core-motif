---
nav_exclude: true
---

# Parse callback input validation

`XmeGetNextCharacter` now treats missing input/output pointers and negative
pattern lengths as a no-op (`XmINSERT`). Valid callbacks retain the existing
component parsing behavior, while malformed application parse callbacks no
longer trigger an assertion or pointer arithmetic on invalid data.

The focused regression check is
`tests/automated/check-next-character-inputs.py`.
