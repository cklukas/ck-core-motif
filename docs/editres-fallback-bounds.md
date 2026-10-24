# EditRes fallback formatting

The edit-resources protocol converts widget resources to strings. When a
resource converter fails, the integer fallback appends the resource type to a
small stack buffer. Resource type names are application-defined and can be
longer than the old 64-byte buffer.

The helper now receives its destination capacity, formats numeric values with
`snprintf`, and appends the type within the remaining space. The protocol
caller uses a larger bounded buffer, preserving normal output while preventing
an arbitrary resource type from overwriting the stack.

`tests/automated/check-editres-fallback-bounds.py` guards the interface and
formatting contracts.
