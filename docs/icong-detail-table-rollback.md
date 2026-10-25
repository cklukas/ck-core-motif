---
nav_exclude: true
---
# IconGadget detail-table rollback

`XmIconGadget` now copies `XmNdetail` tables through one checked helper.  The
helper validates the table byte count against both host `size_t` and Xt's
`Cardinal` range, releases partial `XmString` copies, and reports failure by
returning no table.  During `SetValues`, the old table is released only after
the requested replacement has been copied successfully; failed updates keep
the existing detail data intact.

The focused regression is
`tests/automated/check-icong-detail-table-rollback.py`.
