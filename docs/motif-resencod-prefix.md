# Compound-text custom charset prefix allocation

`processCharsetAndText` constructs an escape prefix for non-standard compound
text character-set tags.  The prefix length is now checked against both the
`unsigned int` output-length contract and `size_t` allocation limits before
allocation.  A failed allocation returns a conversion failure while leaving
the existing output buffer owned by the caller.

The regression check is
`tests/automated/check-resencod-prefix-allocation.py`.
