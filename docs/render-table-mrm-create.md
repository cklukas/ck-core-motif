---
nav_exclude: true
---
# MRM render-table construction

`_XmCreateRenderTable` now checks its table body and handle allocations. A
failed handle allocation releases the body and returns `NULL` before any
render-table fields are initialized.

A null parent is rejected before display lookup for the same reason.
