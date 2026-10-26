# MWM raise-argument traversal

`F_Raise` scans optional family-stacking arguments.  The old loop subtracted
two from `strlen(args)` into an unsigned counter, so a short argument could
underflow into a very large traversal and a very long argument could truncate
the bound.  The loop now uses `size_t` directly and stops on an empty token.

Normal `within` and `freeFamily` arguments retain their existing behavior.
