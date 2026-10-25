# XmString table concatenation inputs

`XmStringTableToXmString` now rejects a null table element rather than
silently omitting it from the concatenated result.  The accumulated string is
freed before returning null, making malformed input an explicit all-or-nothing
failure.
