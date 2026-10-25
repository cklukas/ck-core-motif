# Clipboard copy allocation failure handling

`XmClipboardCopy` now checks allocations for newly created format records and
their data buffers before publishing the item update. It also checks the
status and returned buffer from existing-format retrievals before appending.
Failures release temporary storage and clipboard locks and return
`ClipboardFail` (or the underlying retrieval status), avoiding null-pointer
dereferences and partially published records. Successful copies, including
zero-length and pass-by-name copies, retain their existing behavior.
