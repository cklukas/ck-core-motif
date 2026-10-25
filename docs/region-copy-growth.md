# Region copy growth

`miRegionCopy` now grows the destination rectangle array transactionally. It
rejects a rectangle-count multiplication that cannot fit in `size_t`, keeps
the original destination pointer when `XtRealloc` fails, and updates the
destination only after the new storage is available.

The focused regression is `tests/automated/check-region-copy-growth.py`.
