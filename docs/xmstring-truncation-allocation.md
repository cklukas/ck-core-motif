# XmString ASN.1 truncation allocation failures

`_XmStringTruncateASN1`, used by `XmStringNCopy`, now checks both replacement
and resizing allocations before copying or rewriting the byte stream. On
failure it releases the incomplete stream and returns `NULL`, allowing the
public copy operation to fail cleanly without dereferencing an invalid buffer.
