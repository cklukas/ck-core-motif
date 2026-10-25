---
nav_exclude: true
---
# Mrm widget record offset bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now verify nonzero class
and creation-callback offsets fall within the widget record before using them.
Valid records retain their existing class lookup and callback behavior.

The source-level regression is
`tests/automated/check-mrm-widget-offset-bounds.py`.
