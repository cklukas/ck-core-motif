# Text class translation allocation

Text class initialization now computes the merged translation-table size with
checked `size_t` arithmetic before requesting Xt storage.  If the size cannot
be represented or the allocation fails, fast-subclass registration remains
available and class initialization returns without dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-text-class-translation-allocation.py`.
