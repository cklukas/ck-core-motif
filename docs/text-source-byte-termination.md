# One-byte text-source termination

`_XmTextBytesToCharacters` accepts an `add_null_terminator` flag.  The
one-byte branch now writes the requested terminator into the caller-provided
extra slot, matching the behavior of the 16-bit and wide-character branches.
Callers that pass `False` retain the previous length-only behavior.
