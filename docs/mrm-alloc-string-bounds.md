# Mrm shared string allocation bounds

The shared Mrm string helper now keeps the source length in `size_t`, checks
the terminator-inclusive allocation arithmetic, and copies exactly the
validated string bytes. Existing callers receive the same null-terminated
copy and `NULL` behavior for allocation failure, while malformed sizes cannot
wrap the allocation request.

The source-level regression is
`tests/automated/check-mrm-alloc-string-bounds.py`.
