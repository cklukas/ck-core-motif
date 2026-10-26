# Text line-coordinate bounds

`XtoPosInLine` and `YtoPosInLine` now compute line/metric coordinates in
`int64_t` and saturate them before calling the Position-based `XYToPos`
callback. Large line indices, line heights, widths, or margins therefore
cannot wrap the hit-test coordinate into a different part of the widget.

The focused source contract is checked by
`tests/automated/check-text-line-coordinate-bounds.py`.
