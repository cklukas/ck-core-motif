# XmString segment copy bounds

When a compound-string segment is copied, its character count is now checked
before wide-character multiplication, terminator space, and the Xt allocation
width are computed.  Invalid counts fail through the existing tag/text cleanup
path instead of wrapping the allocation size.

The focused source contract is checked by
`tests/automated/check-xmstring-segment-copy-bounds.py`.
