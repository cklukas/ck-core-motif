# Text-source byte-output termination

`_XmTextCharactersToBytes` promises a NUL-terminated byte stream.  The
one-byte branch now writes that terminator, and the 16-bit branch does so even
when every requested character produced output.  Callers retain responsibility
for reserving the extra byte described by the API contract.
