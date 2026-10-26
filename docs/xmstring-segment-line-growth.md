# XmString segment line growth

When adding a segment beyond the current line range, XmString grows its line
entry array through Xt.  The growth check now covers both `size_t` and Xt's
`Cardinal` byte range before resizing, while retaining the existing cleanup on
allocation failure.
