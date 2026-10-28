# UIL argument-entry allocation failure handling

The argument-pair semantic action now checks its argument-entry allocation
before installing forward-reference patches or publishing the node into the
parse frame.  An exhausted allocator therefore returns through the existing
error-safe parser path without dereferencing a null argument entry.

The source-contract regression is
`tests/automated/check-uil-argument-entry-allocation.py`.
