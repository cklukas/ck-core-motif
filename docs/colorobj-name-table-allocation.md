# ColorObj screen-name table allocation

Color initialization builds a temporary table of screen-name pointers through
`XmStackAlloc`.  That macro narrows heap allocations to an unsigned size, so
the table check now bounds both `size_t` and `UINT_MAX` before multiplication.
Normal screen-name construction is unchanged; an unrepresentable table falls
back to the existing failure path.
