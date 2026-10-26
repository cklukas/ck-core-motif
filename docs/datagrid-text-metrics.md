---
nav_exclude: true
---
# DataGrid text metrics

DataGrid cell and header text is measured with the active XmFontList through
`XmStringExtent`, then vertically centered before `XmStringDraw`. The focused
regression checks this render-table contract rather than the retired direct
`XQueryFont` path.

The check is implemented by
`tests/automated/check-datagrid-text-metrics.py`.
