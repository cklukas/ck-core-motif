# TextField full-GC clip bounds

`SetFullGC` now computes shadow and highlight insets in `uint64_t`. If the
inset consumes the widget dimension, the corresponding clip dimension is
zero; otherwise subtraction happens only after the half-dimension check.
Origins are clamped to the signed X11 rectangle range.

The focused source contract is checked by
`tests/automated/check-textfield-full-gc-bounds.py`.
