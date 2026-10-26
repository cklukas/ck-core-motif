# XmString segment-copy allocation guards

The non-mutating `XmStringConcat` path temporarily copies an existing segment
array when a newline representation must be moved into a cloned string. The
copy now checks the segment count against both `size_t` and Xt `Cardinal`
limits, and handles allocation failure by restoring the original string state
before returning `NULL`.

The focused source-contract check is
`tests/automated/check-xmstring-segment-copy-allocation.py`.
