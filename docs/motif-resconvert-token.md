# Resource-converter token allocation failure

`GetNextToken` now checks both its one-byte terminated-token fallback and its
normal token-buffer allocation before writing.  Allocation failure returns
`NULL` to the existing converter path without dereferencing invalid storage.

The regression check is
`tests/automated/check-resconvert-token-allocation.py`.
