---
nav_exclude: true
---
# TextField highlight-list growth

TextField highlight insertion now bounds the reallocation size and preserves
the existing highlight count/capacity if `XtRealloc()` fails.  The new record
is not written until the resized list is available.

Regression coverage: `tests/automated/check-textfield-highlight-growth.py`.
