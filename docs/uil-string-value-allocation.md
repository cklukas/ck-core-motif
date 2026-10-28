# UIL string-value allocation failures

`create_str_entry` first allocates a semantic node and then its string
payload.  It now checks both results before initialization.  If the payload
allocation fails, the node is placed on the existing semantic free-list and
the failure is reported through Xt, avoiding a null payload dereference and a
leaked node.
