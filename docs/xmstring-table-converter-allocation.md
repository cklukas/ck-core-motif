---
nav_exclude: true
---
# XmString-table conversion allocation

The XmString-table resource converter now checks each per-item string
allocation's input length against Xt's `Cardinal` range, then passes the
accepted size explicitly. Initial table allocation, growth reallocation, and
generated XmString creation are also checked. Any failure releases the
partial table and strings before returning `False`; successful conversion and
normal destroy ownership remain unchanged.
