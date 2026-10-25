# Resource converter token allocation

`GetNextToken` now checks both its empty-token and normal-token allocations
before writing into the returned buffer.  Allocation failure returns `NULL`
without dereferencing invalid storage; callers retain their existing end-of-
input behavior.

The regression check is
`tests/automated/check-resconvert-token-allocation.py`.
