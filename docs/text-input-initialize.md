# Text input initialization

Text widgets require both an input record and its private input data before
keyboard traversal, selection, or preedit callbacks can run.  Allocation
failure now reports an Xt error; the partial record is released and the
widget does not silently continue with missing input state.

The focused source contract is checked by
`tests/automated/check-text-input-initialize.py`.
