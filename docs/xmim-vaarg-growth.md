# XmIm VaArg-list growth

Motif’s input-method adapter builds temporary `XVaNestedList` arguments in a
small growable array.  The old helper updated the capacity and pointer before
checking `XtRealloc`, then wrote the new pair unconditionally.

The helper now checks the capacity-to-byte conversion and commits the new
array only after allocation succeeds.  A failed growth leaves the existing
argument list intact.
