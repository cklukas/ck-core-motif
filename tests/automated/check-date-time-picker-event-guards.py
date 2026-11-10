#!/usr/bin/env python3
"""Ensure date and time picker input ignores insensitive widgets."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, class_name in (
    ("DatePicker.c", "XmIsDatePicker"),
    ("TimePicker.c", "XmIsTimePicker"),
):
    source = (root / name).read_text(encoding="utf-8")
    start = source.index("static void Events(Widget widget")
    body = source[start:source.index("\n}", start) + 2]
    assert class_name in body
    assert "widget->core.being_destroyed || !XtIsSensitive(widget)" in body

print("PASS DatePicker and TimePicker ignore disabled input")
