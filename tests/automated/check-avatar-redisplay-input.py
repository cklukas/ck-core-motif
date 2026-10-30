#!/usr/bin/env python3
"""Ensure Avatar Redisplay validates its widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Avatar.c").read_text()
start = source.index("static void Redisplay(Widget widget")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "widget == NULL || !XtIsSubclass(widget, xmAvatarWidgetClass)" in body
assert body.index("if (widget == NULL") < body.index("XtDisplay(widget)")
print("Avatar Redisplay validates its widget")
