# TimePicker range redraw

`XmTimePickerSetRange` now invalidates a realized picker after successfully
changing its minimum and maximum times.  Range-dependent labels and controls
therefore repaint immediately, matching the DatePicker and Calendar range
APIs, without changing validation or notification behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-timepicker-range-redraw.py
```
