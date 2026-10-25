# Text source conversion cursor bounds

`_XmStringSourceGetString` converts source blocks into either a byte string or
a wide-character string.  Its destination cursor now uses `size_t`, checks
the remaining allocation before each copy or conversion, and rejects invalid
wide-character conversion results.  Large representable ranges therefore
fail cleanly instead of overflowing an `int` cursor or writing beyond the
allocated result.

`tests/automated/check-text-source-cursor-bounds.py` protects these contracts.
