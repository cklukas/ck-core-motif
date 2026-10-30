# XmStringNCopy conversion failure

`XmStringNCopy` first serializes the source string to Motif's ASN.1 form so
it can honor the requested byte limit.  A failed serialization reports zero
length and no buffer; that state is now rejected before the no-truncation
shortcut.  Applications therefore receive `NULL` on allocation/serialization
failure instead of a misleading successful copy.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-ncopy-conversion-failure.py
