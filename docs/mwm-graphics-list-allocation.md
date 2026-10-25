---
nav_exclude: true
---

# MWM graphics-list allocation

MWM frame drawing grows arrays of `XRectangle` values through `RList`. The
list stores its element count in signed `int` fields, while callers provide an
unsigned extension amount. The growth and initial allocation paths now reject
counts that cannot fit those fields or Xt's `Cardinal` allocation width before
performing the size multiplication.

This keeps allocation failure deterministic without changing the public
`AllocateRList` or `ExtendRList` interfaces. The regression is covered by
`tests/automated/check-mwm-graphics-list-allocation.py`.
