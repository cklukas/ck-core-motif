# Text line-count range bounds

The string source line counter now normalizes negative starts and clamps the
requested length before any end-position addition.  This avoids signed and
unsigned wraparound for very large ranges while preserving the existing
Unicode and legacy line-counting contracts.

The focused source contract is checked by
`tests/automated/check-text-line-count-bounds.py`.
