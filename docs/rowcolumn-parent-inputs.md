---
nav_exclude: true
---
# RowColumn creation parent guards

The non-varargs RowColumn creation wrappers now return `NULL` when passed a
null parent, before application-context lookup. This covers row columns,
work areas, radio boxes, option menus, menu bars, popup menus, and pulldown
menus. Valid parent widgets retain the existing creation behavior.

The focused regression is
`tests/automated/check-rowcolumn-parent-inputs.py`.
