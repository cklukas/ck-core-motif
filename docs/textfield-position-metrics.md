# TextField position metrics

`GetXYFromPos` now computes the baseline y-coordinate in `int64_t` and rejects
values outside the signed X11 `Position` range before either Xft or legacy
coordinate paths store it. This prevents large margin/font metrics from
wrapping the insertion-point coordinate.

The focused source contract is checked by
`tests/automated/check-textfield-position-metrics.py`.
