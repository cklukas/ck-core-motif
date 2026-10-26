# MWM client-list sorting bounds

Session-management client sorting now skips invalid null lists and passes its
positive signed client count to `qsort()` as an explicit `size_t`. Valid lists
retain the same workspace ordering while malformed empty inputs are harmless.

The source contract is covered by
`tests/automated/check-mwm-client-sort-bounds.py`.
