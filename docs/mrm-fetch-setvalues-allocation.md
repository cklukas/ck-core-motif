# Mrm set-values allocation handling

`UrmFetchSetValues` now rejects an empty argument list before creating
scratch state, checks the local argument array and pointer-list setup, checks
the reusable resource descriptor allocation, and handles legacy font-list
allocation failure without entering fixup code with a null pointer.

The source-level regression is
`tests/automated/check-mrm-fetch-setvalues-allocation.py`.
