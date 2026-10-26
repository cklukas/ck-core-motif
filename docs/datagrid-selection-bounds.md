---
nav_exclude: true
---
# DataGrid selection bounds

`XmDataGridGetSelectedCell` and `XmDataGridBeginEdit` now validate both sides
of the selected row/column pair against the current model counts. Stale
selection state after a model change is rejected safely instead of indexing
past the row or column arrays; valid selections behave as before. The focused
regression anchors on the function definitions so unrelated call sites cannot
produce a false failure.
