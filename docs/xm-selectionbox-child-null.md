---
nav_exclude: true
---
# XmSelectionBox child lookup null-safety

`XmSelectionBoxGetChild` now returns `NULL` for a null selection-box widget
before Xt app-context handling. Valid child identifiers and returned child
widgets remain unchanged.
