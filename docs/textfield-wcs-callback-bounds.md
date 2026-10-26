# TextField WCS callback buffers

Both TextField modify-verify branches that construct a wide callback block now
validate signed lengths, wide-character multiplication, and Xt allocation
limits.  Allocation failure returns safely; the branch that owns a temporary
byte callback buffer releases it before returning.

The focused source contract is checked by
`tests/automated/check-textfield-wcs-callback-bounds.py`.
