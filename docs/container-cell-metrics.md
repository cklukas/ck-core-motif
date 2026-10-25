# Container cell metrics

The cell-style preferred-size calculation divides child dimensions by the
current cell width and height.  During early resource initialization those
metrics can still be zero.  The calculation now returns without changing the
caller’s requested size until valid metrics are available, avoiding a
process-killing division-by-zero while preserving the normal layout path.

`tests/automated/check-container-cell-metrics.py` protects this guard.
