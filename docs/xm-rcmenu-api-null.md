---
nav_exclude: true
---
# XmRowColumn menu API null-safety

`XmGetPostedFromWidget` and `XmGetTearOffControl` now return `NULL` for null
menus. `XmMenuPosition` returns for a null menu or button event before Xt
app-context handling. Valid menu lookup and positioning behavior remain
unchanged.
