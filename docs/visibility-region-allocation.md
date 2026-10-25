---
nav_exclude: true
---

# Visibility region allocation handling

`XmGetVisibility` now checks all sibling-obscuration region allocations before
using them. If Xlib cannot provide a required region, partially allocated
regions and the child list are released and the function returns the existing
conservative fully-obscured result.

The focused regression check is
`tests/automated/check-visibility-region-allocation.py`.
