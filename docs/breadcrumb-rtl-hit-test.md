---
nav_exclude: true
---
# BreadcrumbBar RTL hit testing

BreadcrumbBar renders segments in reverse order when `XmNbreadcrumbRtl` is
enabled.  Hit testing previously always traversed the forward order, so the
segment selected by a click did not match the segment under the pointer.  The
hit-test path now follows the same traversal as rendering in both directions.

`tests/automated/check-breadcrumb-rtl-hit-test.py` keeps the two traversal
orders aligned.
