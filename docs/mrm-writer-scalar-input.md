# Mrm writer scalar inputs

`UrmCWRSetArgValue` now rejects null pointers for floating-point and color-table
values before dereferencing or sizing them.  Integer and null representation
types retain their existing immediate-value behavior, and string helpers keep
their established null-as-empty handling.

The source-level regression is
`tests/automated/check-mrm-writer-scalar-input.py`.
