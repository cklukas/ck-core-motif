# DataField selection assembly guards

When a compound or multi-value selection is converted before insertion, the
DataField code builds a temporary byte string and, for wide-character fields,
a temporary `wchar_t` buffer. Both allocations are now checked, and the wide
buffer size is bounded by Xt's `Cardinal` limit as well as native arithmetic
limits. Conversion failure releases the incoming selection and temporary data
before reporting an unsuccessful insertion.

The focused source-contract check is
`tests/automated/check-datafield-selection-assembly.py`.
