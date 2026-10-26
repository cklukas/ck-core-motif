---
nav_exclude: true
---
# DataGrid live color updates

`XmDataGrid` now refreshes its drawing GC and requests a repaint when foreground
or background resources change through `SetValues`. Theme changes therefore
update existing data-view widgets without requiring recreation.

The focused source contract is checked by
`tests/automated/check-datagrid-color-updates.py`.
