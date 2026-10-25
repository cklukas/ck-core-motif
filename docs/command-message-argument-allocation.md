---
nav_exclude: true
---
Command and message-dialog argument allocation bounds
======================================================

The `XmCreateCommand` and message-dialog convenience constructors append an
internal `XmNdialogType` argument before creating the widget.  Their public
counts are legacy `Cardinal` or `int` values, while the temporary argument
array is allocated through Xt's byte-counted allocator.  The constructors now
reject counts that cannot represent both the extra element and the resulting
byte size in either interface before doing the multiplication.  This avoids
wraparound and undersized temporary arrays for hostile or malformed counts.
The checked command-constructor byte count is passed explicitly as an Xt
`Cardinal`.

An empty argument list remains valid even when its pointer is null; non-empty
lists still require a valid pointer.  This preserves normal Motif convenience
constructor behavior while avoiding an undefined zero-length `memcpy` call.
