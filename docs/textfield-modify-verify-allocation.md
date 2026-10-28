# TextField modify-verify allocation handling

TextField SetValues now validates and checks the temporary snapshot used by
modify-verify callbacks before copying it.  If an approved replacement cannot
be allocated, the current proposed value is retained instead of dereferencing
a null buffer or freeing the live value.  Oversized and negative lengths are
rejected before multiplication.

The source-contract regression is
`tests/automated/check-textfield-modify-verify-allocation.py`.
