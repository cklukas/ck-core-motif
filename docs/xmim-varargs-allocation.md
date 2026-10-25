# Input-method varargs allocation failures

`XmImVaSetFocusValues` and `XmImVaSetValues` now stop safely when their
temporary argument array cannot be allocated. The existing zero-argument case
continues without allocating an array.
