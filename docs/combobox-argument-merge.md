---
nav_exclude: true
---
# ComboBox argument merging

ComboBox child and convenience constructors merge caller arguments with
internal defaults before creating widgets.  Each merge result is now checked;
allocation failure returns a null widget result instead of passing a null
argument list into Xt.  Successful child construction and argument ownership
are unchanged.

The regression check is `tests/automated/check-combobox-argument-merge.py`.
