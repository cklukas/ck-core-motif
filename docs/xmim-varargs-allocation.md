---
nav_exclude: true
---
# Input-method varargs allocation failures

`XmImVaSetFocusValues` and `XmImVaSetValues` now stop safely when their
temporary argument array cannot be allocated or the varargs counter reaches
its signed representation limit. The existing zero-argument case continues
without allocating an array, and validated counts are explicitly converted to
the public `Cardinal` argument count.
