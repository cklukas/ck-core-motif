---
nav_exclude: true
---
Input-method registration cache growth
=======================================

When the input-method registration cache records a new resource, its `Arg`
array grows by one element.  The growth path now checks the resulting byte
count against both `size_t` and Xt's `Cardinal` limits before calling
`XtRealloc`, preserving the existing failure behavior for unrepresentable or
unallocatable growth.
