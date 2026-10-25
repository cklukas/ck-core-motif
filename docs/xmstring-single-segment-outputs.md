# XmString single-segment output validation

`_XmStringSingleSegment` now validates both output pointers before clearing or
writing them. Invalid callers receive `False` without dereferencing a null
output address.

The source contract is covered by
`tests/automated/check-xmstring-single-segment-outputs.py`.
