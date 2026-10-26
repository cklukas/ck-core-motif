---
nav_exclude: true
---
# XmScaleSetTicks input and allocation bounds

`XmScaleSetTicks` now rejects a non-positive `big_every` interval before it can
divide by zero or generate invalid tick counts.  The scale range is promoted
before subtraction, so using the full signed `int` range cannot overflow.

The medium- and small-tick counts are checked against `INT_MAX`, and the final
separator count is checked against both the host `size_t` and Motif's
`Cardinal` limits before the temporary separator array is allocated.  Normal
tick generation is unchanged; only invalid or unrepresentable requests return
without creating ticks.

`tests/automated/check-scale-ticks-bounds.py` protects this contract.
