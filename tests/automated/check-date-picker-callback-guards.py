#!/usr/bin/env python3
"""Ensure DatePicker calendar callbacks validate both widget inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void calendar_changed(Widget calendar")
body = source[start : source.index("\nstatic void Initialize", start)]
guard = "!XmIsDatePicker((Widget)picker)"
assert guard in body
assert "!XmIsCalendar(calendar)" in body
assert body.index(guard) < body.index("XmCalendarGetDate")
print("PASS DatePicker calendar callbacks validate picker and calendar widgets")
