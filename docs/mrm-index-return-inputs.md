# Mrm index item lookup inputs

`Idb__INX_ReturnItem` now rejects missing file handles, index strings, and
output data handles before traversing the index record.

The source-level regression is `tests/automated/check-mrm-index-return-inputs.py`.
