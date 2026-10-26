# XmPicture allocation guards

`XmParsePicture` builds a regular-expression state machine by growing a node
table. The parser now checks node and transition allocation, rejects table-size
overflow before doubling the table, and marks the parse as failed if a growth
operation cannot be completed. The public constructor returns `NULL` for an
oversized or unrepresentable picture instead of dereferencing a failed node.

The source-contract regression is
`tests/automated/check-picture-allocation-guards.py`; native builds should also
compile `lib/Xm/Picture.c` with the normal Motif warning flags.
