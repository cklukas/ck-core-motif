# TextField clip-rectangle bounds

`GetRect` now computes text margins, shadow thickness, and highlight thickness
in `uint64_t` before subtracting them from widget dimensions. Oversized margins
produce a zero content rectangle instead of wrapping through `Dimension`; the
signed `XRectangle` origins are clamped to their representable range.

The focused source contract is checked by
`tests/automated/check-textfield-rect-bounds.py`.
