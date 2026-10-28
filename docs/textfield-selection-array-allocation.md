# TextField selection-array replacement

TextField SetValues now allocates and copies a replacement selection array
before releasing the previous array or publishing the new pointer.  Invalid
sizes and allocation failure leave the existing selection state intact rather
than copying through a null pointer or losing the old array.

The source-contract regression is
`tests/automated/check-textfield-selection-array-allocation.py`.
