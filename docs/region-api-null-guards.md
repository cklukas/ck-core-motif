---
nav_exclude: true
---
# XmRegion API null guards

`_XmRegionDestroy` now accepts a `NULL` region.  `_XmRegionGetRectangles`
validates both output pointers and treats a `NULL` region as an empty result,
so optional geometry cleanup and output collection cannot dereference absent
state.

The focused regression is `tests/automated/check-region-api-null-guards.py`.
