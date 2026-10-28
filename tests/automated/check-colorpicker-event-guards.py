#!/usr/bin/env python3
"""Ensure ColorPicker event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\nstatic Boolean SetValues", start)]
assert "if (picker == NULL || event == NULL || !XmIsColorPicker(widget))" in body
print("ColorPicker event callbacks validate widget and event inputs")
