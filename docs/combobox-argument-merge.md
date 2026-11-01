---
nav_exclude: true
---
# ComboBox argument merging

ComboBox child and convenience constructors merge caller arguments with
internal defaults before creating widgets.  The shared merge helper rejects a
`Cardinal` count that would wrap, and allocation failure returns a null widget
result instead of passing a null argument list into Xt.  The edit-box path also
releases its temporary selected-item string on a failed merge and waits for a
successful child before inheriting its font.  Successful construction and
argument ownership are unchanged.

The regression check is `tests/automated/check-combobox-argument-merge.py`.
