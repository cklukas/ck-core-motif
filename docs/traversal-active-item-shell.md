---
nav_exclude: true
---

# Traversal active-item and shell validation

`_XmGetActiveItem` now returns `NULL` for a missing widget, and focus-moved
dispatch returns `False` when no top-level shell can be resolved. Valid focus
lookups and vendor-shell callback dispatch remain unchanged.

The focused regression check is
`tests/automated/check-traversal-active-item-shell.py`.
