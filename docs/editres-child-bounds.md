# Edit-resources child enumeration bounds

The edit-resources protocol enumerates normal and popup children together.
`FindChildren` now checks each count addition, validates the combined byte
count against both `size_t` and Xt's `Cardinal` width, and reports an empty
result when allocation fails. Existing callers continue to receive the same
child array and count for ordinary widget trees.
