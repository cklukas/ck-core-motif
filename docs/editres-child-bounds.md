# Edit-resources child enumeration bounds

The edit-resources protocol enumerates normal and popup children together.
The enumeration no longer builds a child array: `ChildCount` saturates the sum
of the two counts and `ChildAt` returns one child by index, normal children
first, then popup children, so there is no allocation to fail and no byte
count to overflow. The widget lookup and the tree dump use the same two
helpers. See [Editres protocol handler](editres-wire-protocol.md).
