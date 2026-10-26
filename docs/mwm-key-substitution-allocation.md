# MWM key-substitution allocation

The optional WSM key-substitution table is built from configuration text.
`InitKeySubs` now checks its table, source-string, and replacement-string
allocations. Reallocation uses a temporary pointer, and allocation failure
publishes only the already-complete entries with their correct count. A failed
replacement allocation releases its paired source string before returning.

The behavior is covered by
`tests/automated/check-wm-resparse-keysubs-allocation.py`.
