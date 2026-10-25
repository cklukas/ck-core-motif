# List translation allocation failure

List class initialization combines its built-in translation strings in a
temporary local buffer.  The local allocation is now checked before copying.
If it fails, class initialization returns safely instead of dereferencing a
null buffer.

The regression check is
`tests/automated/check-list-translation-allocation.py`.
