# UIL token value allocation failures

`sem_create_value_entry` creates the semantic node for token values and, for
string types, allocates a payload.  The node and both narrow/compound payload
allocations are now checked.  Failed payloads release the node and return the
existing semantic error value instead of allowing a null copy destination.
