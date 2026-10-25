---
nav_exclude: true
---

# MWM action-argument reallocation

`f.action` parsing can expand environment-backed arguments and grow its
argument vector. The parser now keeps the old allocation valid until a new
allocation succeeds, so allocation failure cannot turn the subsequent copy
into a null-pointer write. The event cleanup path also releases the argument
vector after freeing its strings.

Successful action parsing and the existing argument/resource interfaces are
unchanged. The focused regression check is
`tests/automated/check-mwm-action-arg-realloc.py`.
