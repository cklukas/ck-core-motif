# Mrm icon loader input validation

`Urm__CW_LoadIconImage` fixes up an icon record, relocates its color table,
and appends referenced resource contexts. It now rejects a missing icon,
serialized buffer, or context list before reading the validation field or
performing pointer arithmetic.

This turns malformed or incomplete literal contexts into `MrmFAILURE` instead
of allowing an immediate null dereference. Valid loader calls retain the same
relocation and resource-loading behavior. The source-level regression is
`tests/automated/check-mrm-icon-loader-inputs.py`.
