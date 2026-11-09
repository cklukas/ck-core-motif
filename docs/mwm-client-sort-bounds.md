# MWM client-list sorting bounds

Session-management client sorting now skips invalid null lists and passes its
positive signed client count to qsort as an explicit size_t. Its comparator also
orders missing client, screen, or active-workspace state after valid entries
without dereferencing incomplete records. Valid lists retain the same workspace
ordering while malformed inputs are harmless.

The source contract is covered by
`tests/automated/check-mwm-client-sort-bounds.py`.
