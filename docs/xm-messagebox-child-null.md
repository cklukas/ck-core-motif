---
nav_exclude: true
---
# XmMessageBox child lookup null-safety

`XmMessageBoxGetChild` now returns `NULL` for a null widget before Xt
app-context lookup. Valid child identifiers and message-box child results are
unchanged.
