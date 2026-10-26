# TextField size bounds

`ComputeSize` now performs text, column, font-metric, and margin arithmetic in
`uint64_t`. Results larger than the X11 `Dimension` range are saturated rather
than wrapping to a small width or height.

The focused source contract is checked by
`tests/automated/check-textfield-size-bounds.py`.
