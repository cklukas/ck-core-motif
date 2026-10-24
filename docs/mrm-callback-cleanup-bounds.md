# Mrm callback cleanup bounds

Callback-value cleanup now tolerates a null or empty ownership list and limits
iteration to complete type/value pairs.  If an allocation or append failure
leaves a final type tag without a value, that tag is discarded before the list
is released, avoiding an out-of-bounds read while retaining normal callback
value destruction.

The source-level regression is
`tests/automated/check-mrm-callback-cleanup-bounds.py`.
