---
nav_exclude: true
---
# DataGrid render-table rollback

When a live DataGrid receives a new render table, it now keeps the previous
valid table if `XmFontListCopy` cannot allocate a replacement. The old table is
released only after a successful copy, preventing a transient allocation
failure from leaving the widget unable to render text.

The focused source contract is checked by
`tests/automated/check-datagrid-render-table-rollback.py`.
