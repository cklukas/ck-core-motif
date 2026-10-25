---
nav_exclude: true
---
# List result allocation failures

`XmListGetMatchPos` and `XmListGetSelectedPos` now check their caller-owned
result allocations against both host `size_t` and Xt's `Cardinal` limits before
writing or copying positions. On allocation failure or an unrepresentable
result, both APIs leave the initialized output pointers and counts unchanged
and return `False`; successful result contents and ownership remain unchanged.
