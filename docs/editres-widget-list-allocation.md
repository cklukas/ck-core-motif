---
nav_exclude: true
---
# Edit-resources widget-list allocation

The edit-resources protocol helper now validates the hierarchy count against
both the 16-bit wire field and Xt's `Cardinal` allocation width, and returns
safely if its temporary widget allocation fails. Successful protocol
serialization is unchanged.

The focused regression is
`tests/automated/check-editres-widget-list-allocation.py`.
