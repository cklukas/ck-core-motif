---
nav_exclude: true
---
# DrawUtils bevel-buffer growth

`XmDrawBevel` uses a static rectangle buffer for small bevels and a dynamic
buffer for larger ones.  The previous growth path updated its capacity before
checking `XtRealloc`, so allocation failure could leave a null buffer that
the drawing loop immediately dereferenced.

The dynamic path now checks the byte count against both host `size_t` and Xt
`Cardinal` limits, allocates a replacement, and commits the pointer and
capacity only after success. The existing buffer is retained when growth
cannot be completed.
