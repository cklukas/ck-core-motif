#!/usr/bin/env python3
"""Ensure Stepper event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\nstatic Boolean SetValues", start)]
assert "if (s == NULL || event == NULL || !XmIsStepper(widget)) return;" in body
print("Stepper event callbacks validate widget and event inputs")
