# Mrm writer vector inputs

The UID writer now rejects a null string-vector pointer when its element count
is nonzero in both character and compound-string setters.  Empty vectors keep
their existing behavior, and individual null elements remain valid inputs for
the existing append helpers.

The source-level regression is
`tests/automated/check-mrm-writer-vector-input.py`.
