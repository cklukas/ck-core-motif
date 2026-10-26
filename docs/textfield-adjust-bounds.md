# TextField AdjustText geometry bounds

`AdjustText` now computes margin and shadow/highlight thickness totals in
`uint64_t`. A widget with no usable content width returns without scrolling,
and fill-rectangle heights and origins are narrowed only after their bounds
are established.

The focused source contract is checked by
`tests/automated/check-textfield-adjust-bounds.py`.
