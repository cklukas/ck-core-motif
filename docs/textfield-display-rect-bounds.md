# TextField display-rectangle bounds

`TextFieldGetDisplayRect` now validates its output pointer and computes all
margin and shadow arithmetic in `uint64_t`. Overlarge margins produce a zero
content rectangle, while origins are clamped to the signed `XRectangle`
coordinate range instead of wrapping through `Position`.

The focused source contract is checked by
`tests/automated/check-textfield-display-rect-bounds.py`.
