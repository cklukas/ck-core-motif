#!/usr/bin/env python3
"""Ensure DatePicker event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\n}\n", start)]
assert "if (picker == NULL || event == NULL || !XmIsDatePicker(widget))" in body
print("DatePicker event callbacks validate widget and event inputs")
