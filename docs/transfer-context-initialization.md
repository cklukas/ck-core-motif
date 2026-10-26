# Initialize transfer conversion contexts

Motif keeps per-selection conversion state in a shared hash table.  Newly
created records now initialize all flags, operation state, item identity,
location, client data, and drag-context pointers before publishing the record
to the table.  Key and context allocation failures are also handled without
inserting a partial record.

Existing conversion state and selection behavior are unchanged.  The focused
source regression is
`tests/automated/check-transfer-context-initialization.py`.
