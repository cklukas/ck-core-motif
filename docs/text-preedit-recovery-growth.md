# Text preedit recovery growth

Overstrike recovery now bounds the appended byte count and commits its
`XtRealloc` only after success.  A failed resize or unrepresentable recovery
size frees the old buffer and resets preedit state instead of dereferencing a
null pointer or appending past the allocation.

The focused source contract is checked by
`tests/automated/check-text-preedit-recovery-growth.py`.
