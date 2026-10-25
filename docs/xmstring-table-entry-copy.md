# XmString table entry copying

`XmStringTableToXmString` now checks each table-entry copy before
concatenation.  A failed copy releases the accumulated result and returns
null, rather than being interpreted as an optional missing operand.
