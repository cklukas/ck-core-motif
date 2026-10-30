---
nav_exclude: true
---
# RowColumn popup allocation

RowColumn menu creation now rejects null parent or name inputs and checks the
popup-shell result before marking the shell private.  A failed popup-shell
allocation returns cleanly instead of dereferencing a null menu-shell widget.
