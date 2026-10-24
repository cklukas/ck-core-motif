# Compound-string segment size bounds

When a compound-string segment is returned with indentation or a separator,
the converter allocates a new buffer and prepends tabs or appends a newline.
The old code stored the resulting length in an `int`, even though the source
length is `size_t`.  A large segment could therefore wrap the length and cause
an undersized allocation before the copy.

The conversion now keeps the length in `size_t` and rejects an unrepresentable
prefix-plus-segment size as an invalid segment before allocating.  Ordinary
compound strings retain the same bytes and separator behavior.  The source
regression in `tests/automated/check-compound-segment-bounds.py` protects the
checked arithmetic and the non-overflowing type.
