#!/usr/bin/env python3
"""Ensure TimePicker event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TimePicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\n}\n", start)]
assert "if (picker == NULL || event == NULL || !XmIsTimePicker(widget))" in body
print("TimePicker event callbacks validate widget and event inputs")
