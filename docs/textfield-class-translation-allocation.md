# TextField class translation allocation

TextField class initialization merges three translation fragments.  The
merged size is now checked with `size_t` arithmetic and Xt's allocation limit;
fast-subclass registration remains intact when the optional merged table cannot
be allocated.

The focused source contract is checked by
`tests/automated/check-textfield-class-translation-allocation.py`.
