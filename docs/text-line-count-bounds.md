# Text line-count range bounds

The string source line counter now normalizes negative starts and clamps the
requested length before any end-position addition.  Both Unicode and legacy
paths traverse the bounds-checked source-character accessor, avoiding the old
split-gap pointer arithmetic and `int` truncation for very large ranges while
preserving the existing newline contract.

The focused source contract is checked by
`tests/automated/check-text-line-count-bounds.py`.

The counter also returns zero when its source or private data has already been
cleared, avoiding a teardown-time dereference before range normalization.

The newline-character helper also validates its widget and newline storage, so
partially initialized sources cannot be dereferenced while counting.

The source `Scan` callback uses the same entry guard and returns position zero
when its widget list is unavailable. The focused scan contract is checked by
`tests/automated/check-text-source-scan-inputs.py`.
