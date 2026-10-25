---
nav_exclude: true
---
# Mrm reader arg-list count bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now reject argument-list
counts or extra-runtime counts above `RGMListSizeMax` before size arithmetic,
allocation, and argument iteration. Valid legacy and current arg lists retain
their existing behavior.

The source-level regression is
`tests/automated/check-mrm-arglist-reader-counts.py`.
