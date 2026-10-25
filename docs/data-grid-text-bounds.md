---
nav_exclude: true
---

# Data-grid text-length bounds

`XmDataGrid` now bounds cell and header text lengths before passing them to
`XDrawString`. Normal rendering is unchanged; unusually long strings no longer
wrap the signed Xlib length argument.

The focused regression check is
`tests/automated/check-data-grid-text-bounds.py`.
