# TextField baseline bounds

`XmTextFieldGetBaseline` now computes its margin, shadow, highlight, and font
ascent total in `int64_t` before returning the legacy `int` result. Values
outside the public return range are clamped instead of wrapping due to
intermediate `Dimension` arithmetic.

The focused source contract is checked by
`tests/automated/check-textfield-baseline-bounds.py`.
