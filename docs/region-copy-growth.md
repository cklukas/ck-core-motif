# XmRegion copy growth

`miRegionCopy` now grows a destination rectangle array through a temporary
pointer and checks the element-count multiplication against both `size_t` and
Xt's `Cardinal` byte-size range.  If `XtRealloc` fails,
the destination keeps its existing storage and the copy returns without
corrupting the region.

The copy path also allocates storage when a destination region starts with a
NULL rectangle pointer, so empty or partially initialized regions cannot reach
a NULL `memmove`.

The focused regression is `tests/automated/check-region-copy-growth.py`.
