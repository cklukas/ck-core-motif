---
nav_exclude: true
---
# XmIm VaArg-list growth

VaArg-list expansion now checks both host `size_t` and Xt `Cardinal` byte
limits before calling `XtRealloc`.  If the requested capacity is not
representable, the existing list remains unchanged and the new argument is
ignored, matching the existing allocation-failure behavior.

Motif’s input-method adapter builds temporary `XVaNestedList` arguments in a
small growable array.  The old helper updated the capacity and pointer before
checking `XtRealloc`, then wrote the new pair unconditionally.

The helper now checks the capacity-to-byte conversion and commits the new
array only after allocation succeeds.  A failed growth leaves the existing
argument list intact.
