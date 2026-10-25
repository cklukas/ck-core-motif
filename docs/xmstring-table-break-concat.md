# XmString table break concatenation

`XmStringTableToXmString` now checks the concatenation that adds each break
component.  A failed allocation returns null immediately instead of allowing a
later iteration to continue from a missing accumulated result.
