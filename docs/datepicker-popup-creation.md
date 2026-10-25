---
nav_exclude: true
---
# DatePicker popup creation

The DatePicker now checks lazy popup-shell and calendar creation before using
either widget.  If either creation fails, any partial shell is destroyed and
the picker remains closed; successful popup creation and calendar behavior are
unchanged.

`tests/automated/check-datepicker-popup-creation.py` protects this path.
