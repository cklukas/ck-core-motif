# UIL keysym coercion allocation failure handling

Integer-to-keysym argument coercion now checks the two-byte temporary string
allocation before copying the value or changing its semantic type.  When the
allocation fails, the existing integer value remains intact and validation
continues through the normal type-error path instead of copying through a null
pointer.

The source-contract regression is
`tests/automated/check-uil-keysym-coercion-allocation.py`.
