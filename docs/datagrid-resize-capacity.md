---
nav_exclude: true
---
# DataGrid resize capacity

`XmDataGrid` derives visible-row capacity from its allocated height and uses
that capacity to clamp `XmNdataGridTopRow`.  Its core resize method now
recomputes that state after a parent geometry change.  The `SetValues` path
also requests repaint when width or height changes, keeping the header and
cell-column geometry current even when the resize originates in Xt resource
updates.
