#!/usr/bin/env python3
"""Check DatePicker range updates commit only after the calendar child succeeds."""

from pathlib import Path


source = Path("lib/Xm/DatePicker.c").read_text()
start = source.index("Boolean XmDatePickerSetRange(")
body = source[start:source.index("Boolean XmDatePickerGetRange(", start)]

calendar_update = body.index("XmCalendarSetRange(picker->date_picker.calendar")
minimum_assignment = body.index("picker->date_picker.minimum = *minimum;")
maximum_assignment = body.index("picker->date_picker.maximum = *maximum;")
assert calendar_update < minimum_assignment < maximum_assignment
assert "return False;" in body[calendar_update:minimum_assignment]

print("DatePicker range updates commit parent state after calendar success")
