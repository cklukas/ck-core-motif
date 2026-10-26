# Text line-count range bounds

The string source line counter now normalizes negative starts and clamps the
requested length before any end-position addition.  Both Unicode and legacy
paths traverse the bounds-checked source-character accessor, avoiding the old
split-gap pointer arithmetic and `int` truncation for very large ranges while
preserving the existing newline contract.

The focused source contract is checked by
`tests/automated/check-text-line-count-bounds.py`.
