# UIL callback-entry allocation failure handling

The callback reason/procedure binding action now checks its callback-entry
allocation before installing forward-reference patches or publishing the
entry in the parse frame.  Allocation failure returns through the existing
error-safe parser path without dereferencing a null callback node.

The source-contract regression is
`tests/automated/check-uil-callback-entry-allocation.py`.
