# XmStack growth bounds

The internal XmStack grows by a fixed increment.  Its old push path formed
`top + 1` and `alloc + STACK_INC` in signed `int` fields without checking for
overflow.  A wrapped capacity could then produce a too-small allocation and
an out-of-bounds write.

Push now rejects invalid or exhausted counters, checks the new capacity in
both `size_t` and Xt's `Cardinal` byte-size range, and commits the capacity
only after `XtRealloc` succeeds.  The stack API and normal growth behavior
remain unchanged on platforms where the allocation is representable.
