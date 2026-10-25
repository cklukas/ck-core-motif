# XmString table break-component copying

When a non-null break component is supplied, `XmStringTableToXmString` now
checks its copy before concatenation.  A failed copy releases the accumulated
result and returns null; a null break component remains the documented
separator-free option.
