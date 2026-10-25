# XmString byte-stream allocation failures

`XmCvtXmStringToByteStream` now checks component-size arithmetic and the output
allocation before writing the encoded stream. On overflow or allocation failure
it clears the caller's output pointer and returns zero; successful encoding and
the size-only query remain unchanged.
