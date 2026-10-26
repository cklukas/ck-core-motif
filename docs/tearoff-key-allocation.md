# Tear-off cancel-key allocation guards

Menu tear-off placement resolves the virtual Cancel keys into a temporary
`KeyCode` array. The count is now validated against both `size_t` and Xt's
`Cardinal` limits, and allocation failure exits before the array is populated.
Invalid key-resolution results therefore abort placement cleanly rather than
causing a NULL write or wrapped allocation.

The focused source-contract check is
`tests/automated/check-tearoff-key-allocation.py`.
