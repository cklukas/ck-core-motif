# Mrm reader text-vector validation

`Urm__CW_FixupValue` now rejects null, invalid, or over-sized text vectors
before swapping entries or walking their offsets. Valid Char8 and compound
string vectors retain the existing fixup behavior, while malformed resource
records fail cleanly instead of causing an out-of-bounds traversal.

The source-level regression check is
`tests/automated/check-mrm-reader-text-vector-validation.py`.
