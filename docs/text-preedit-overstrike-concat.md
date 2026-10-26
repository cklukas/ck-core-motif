# Text preedit overstrike concatenation

Overstrike preedit recovery now checks the source string returned for the
remaining text, bounds the combined byte count, and commits the resized buffer
only after `XtRealloc` succeeds.  Failed source retrieval, size arithmetic, or
allocation resets the preedit state without dereferencing invalid storage.

The focused source contract is checked by
`tests/automated/check-text-preedit-overstrike-concat.py`.
