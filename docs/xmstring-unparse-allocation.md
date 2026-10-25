---
nav_exclude: true
---
# XmString unparse allocation safety

`unparse_text` now checks temporary conversion buffers, result growth, and
length arithmetic before copying or converting a component.  A failed or
unrepresentable component is skipped without replacing the existing result
buffer, so malformed or allocation-limited compound strings do not cause a
null dereference or wrapped allocation.
