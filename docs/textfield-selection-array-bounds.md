# TextField selection-array copy bounds

TextField initialization copies the public selection-scan array into owned
storage.  The count is now checked before multiplication and Xt allocation,
and the widget field is replaced only after a successful copy.  Allocation
failure therefore avoids both null `memcpy` and losing the original resource
pointer.

The focused source contract is checked by
`tests/automated/check-textfield-selection-array-bounds.py`.
