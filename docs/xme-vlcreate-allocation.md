XmeVLCreateWidget argument-list allocation
===========================================

`XmeVLCreateWidget` is the shared implementation behind many Motif
variable-argument convenience constructors.  It now rejects negative or
unrepresentable counts before multiplying by `sizeof(Arg)`, avoids an
unnecessary zero-byte allocation for an empty list, and returns `NULL` if Xt
cannot allocate the temporary list.  Allocation failure also releases the
application lock before returning.

The successful varargs conversion and widget-creation ABI are unchanged.
