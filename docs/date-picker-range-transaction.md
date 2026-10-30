---
nav_exclude: true
---
# DatePicker range updates

`XmDatePickerSetRange` updates the optional calendar child before committing the
new range to the DatePicker itself.  If the child rejects the range, the API
returns `False` and the parent retains its previous minimum and maximum rather
than exposing a partially applied update.

Successful updates keep the calendar and DatePicker ranges synchronized.
