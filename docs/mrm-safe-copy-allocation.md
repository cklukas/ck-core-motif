# Mrm safe value-copy allocation

`Urm__CW_SafeCopyValue` creates owned copies for callback and resource values.
Each primitive, string, vector, compound-string, and wide-character allocation
now has a failure guard.  Failed copies leave the original value untouched and
return to the caller instead of dereferencing a null pointer.  Wide-character
size multiplication is checked before allocation as well.

The source-level regression is
`tests/automated/check-mrm-safe-copy-allocation.py`.
