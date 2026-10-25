---
nav_exclude: true
---
# Mrm reader arg-list offset bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now verify that a nonzero
`arglist_offs` points inside the widget record, after its header and before the
record end. Malformed offsets fail before descriptor fields are read.

The source-level regression is
`tests/automated/check-mrm-arglist-offset-bounds.py`.
