# WSM empty-collection decoding

The WSM wire protocol represents empty extension, state, property, match, and
window-entry lists with a zero count. They are valid protocol values, not an
allocation failure.

The request and reply decoders now initialize each corresponding pointer to
`NULL` and continue decoding when the count is zero. They allocate only for a
nonempty list, so platforms where `XtMalloc(0)` returns `NULL` no longer reject
an otherwise valid message or stop before a later field is consumed. Nonempty
lists retain the existing checked-width allocation and failure cleanup paths.

`tests/automated/check-wsm-empty-collection-unpack.py` verifies each affected
request/reply path and the `WM_WINDOWS` continuation contract.
