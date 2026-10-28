#!/usr/bin/env python3
"""Ensure CommandPalette event dispatch validates inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("static void events(Widget widget")
body = source[start:source.index("\nstatic void invoke_callback", start)]
assert "if (widget == NULL || event == NULL || !XmIsCommandPalette(widget)) return;" in body
print("CommandPalette event callbacks validate widget and event inputs")
