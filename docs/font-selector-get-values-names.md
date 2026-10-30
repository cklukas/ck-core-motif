---
nav_exclude: true
---

# FontSelector value-hook resource names

`GetValuesHook` now skips requests with a null resource name as well as null
output pointers. This prevents malformed `Arg` records from reaching string
comparison while leaving valid resource lookup unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-names.py`.
