---
nav_exclude: true
---

# Public part-offset resolution guards

`XmeResolvePartOffsets` is a public compatibility entry point used while
widget classes are assembled. It now rejects null inputs, checks the
superclass-count multiplication against both host `size_t` and Xt's
`Cardinal` width, and releases a successfully allocated normal table if the
optional constraint table cannot be allocated. Resource offsets are not
updated from a partial table.

The focused regression check is
`tests/automated/check-resolve-part-offset-allocation.py`.
