# DataGrid begin-edit rollback

`XmDataGridBeginEdit` now allocates the selected cell's edit text before
releasing an existing edit buffer or error. If allocation fails, the current
editing state remains intact and the operation reports failure.

The focused source regression is
`tests/automated/check-datagrid-begin-edit-rollback.py`.
