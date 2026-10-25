# XmRegion copy growth

`miRegionCopy` now grows a destination rectangle array through a temporary
pointer and checks the element-count multiplication.  If `XtRealloc` fails,
the destination keeps its existing storage and the copy returns without
corrupting the region.

The focused regression is `tests/automated/check-region-copy-growth.py`.
