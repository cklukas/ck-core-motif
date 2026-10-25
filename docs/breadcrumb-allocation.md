---
nav_exclude: true
---
# BreadcrumbBar segment allocation handling

`XmBreadcrumbBarSetSegments` now checks the segment-array size and every item
label allocation.  Partial label arrays are released and the current index is
reset when replacement cannot be completed, preventing later drawing or input
from observing invalid item storage.  Successful segment replacement keeps the
existing selection behavior.

The regression check is `tests/automated/check-breadcrumb-allocation.py`.
