# SegmentedControl item-count range

`XmSegmentedControlSetItems` accepts a `Cardinal` count, while the public
selected-index and keyboard-navigation APIs use signed `int` indices.  A
count above `INT_MAX` therefore cannot be represented safely by the control’s
selection state and would make its internal index casts implementation
dependent.  The setter now rejects such a count before allocation or child
creation; all representable counts retain their existing behavior.

The regression contract is checked by
`tests/automated/check-segmented-count-range.py`.
