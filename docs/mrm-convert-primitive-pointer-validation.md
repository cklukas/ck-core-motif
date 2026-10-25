---
nav_exclude: true
---
# Mrm primitive conversion pointer validation

`Urm__CW_ConvertValue` now rejects missing color descriptors, wide-character
entries, unit values, and string-backed translation, class-name, and keysym
values before dereferencing or passing them to X/Motif helpers. Valid resource
values retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-convert-primitive-pointer-validation.py`.
