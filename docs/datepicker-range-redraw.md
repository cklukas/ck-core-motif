# DatePicker range redraw

`XmDatePickerSetRange` now invalidates a realized picker after successfully
changing its minimum and maximum dates.  Range-dependent labels and controls
therefore repaint immediately, matching the calendar range API, without
changing the existing validation or notification contract.

The focused source-contract check is:

```sh
python3 tests/automated/check-datepicker-range-redraw.py
```
