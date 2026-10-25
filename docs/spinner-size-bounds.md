# Spinner preferred-size bounds

Spinner's preferred width and height include label metrics and twice the
spacing resource.  The old additions were performed in `Dimension` and could
wrap when applications supplied large spacing or font metrics.  Initialization
and `QueryGeometry` now compute in `size_t` and saturate at the maximum
representable `Dimension`.

`tests/automated/check-spinner-size-bounds.py` keeps the bounded sizing helper
in both paths.
