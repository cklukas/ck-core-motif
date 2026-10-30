#!/usr/bin/env python3
"""Check DatePicker resource updates synchronize an existing calendar popup."""

from pathlib import Path


source = Path("lib/Xm/DatePicker.c").read_text()
start = source.index("static Boolean SetValues(")
body = source[start:source.index("static void Redisplay(", start)]

assert "XmCalendarGetDate(picker->date_picker.calendar" in body
assert "XmCalendarSetDate(picker->date_picker.calendar" in body
assert "picker->date_picker.date = old->date_picker.date;" in body
assert "False)))" in body

print("DatePicker resource updates synchronize an existing calendar child")
