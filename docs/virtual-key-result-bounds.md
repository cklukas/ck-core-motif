---
nav_exclude: true
---
# Virtual-key lookup result bounds

`XmeVirtualToActualKeysyms` returns its match count as `int` and allocates an
Xt-owned `XmKeyBindingRec` result array. It now refuses an unrepresentable
match count and validates native and Xt allocation-byte limits before creating
the result. Existing successful lookup and caller ownership behavior are
unchanged.

The focused regression check is
`tests/automated/check-virtual-key-result-bounds.py`.
