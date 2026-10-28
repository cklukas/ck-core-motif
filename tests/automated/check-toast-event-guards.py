#!/usr/bin/env python3
"""Ensure Toast event dispatch validates widget and event inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toast.c").read_text(
    encoding="utf-8"
)
start = source.index("static void events(Widget widget")
body = source[start:source.index("\n", source.index("} }", start))]
assert "if (widget == NULL || event == NULL || !XmIsToast(widget)) return;" in body
print("Toast event callbacks validate widget and event inputs")
