# UTF-8 conversion bounds

`_XmUtf8ToUcs2` is used by text and compound-string rendering paths.  Its
legacy parser assumed that every 2- or 3-byte sequence was complete and that
`seg_len * sizeof(XChar2b)` could not overflow.  A truncated or malformed
segment could therefore read past the supplied buffer, and an extreme length
could wrap the allocation size.

The helper now tracks remaining bytes, validates continuation bytes before
reading them, consumes malformed input as `?`, and checks the allocation
product.  Valid UTF-8 conversion remains unchanged.  The source regression in
`tests/automated/check-utf8-conversion-bounds.py` protects both bounds.
