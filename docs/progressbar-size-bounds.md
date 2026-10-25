# ProgressBar preferred-size bounds

ProgressBar's preferred width and height include label metrics and twice the
spacing resource.  The old additions were performed in `Dimension` and could
wrap for large spacing or font metrics.  Initialization and `QueryGeometry`
now compute in `size_t` and saturate at the maximum representable
`Dimension`.

`tests/automated/check-progressbar-size-bounds.py` keeps the bounded sizing
helper in both paths.
