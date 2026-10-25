# Xm geometry comparison fixes

`XmCompareXtWidgetGeometryToWidget` now compares requested width with the
widget width, and both geometry comparison helpers reject null arguments.
Existing request-mode comparisons remain unchanged.

The source-level regression is `tests/automated/check-xm-geometry-compare.py`.
