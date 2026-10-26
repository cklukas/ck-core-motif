# MWM default-color cache growth

The MWM default-color cache grows as new screen/colormap combinations are
seen. Growth now checks the signed counter and Xt allocation-size limits, and
uses a temporary pointer before publishing the larger cache. Failed growth
therefore leaves the existing cache available to subsequent calls.

The behavior is covered by
`tests/automated/check-mwm-default-colors-allocation.py`.
