#!/usr/bin/env python3
"""Ensure Badge Redisplay validates its widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Badge.c").read_text()
start = source.index("static void Redisplay(Widget widget")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass)" in body
assert body.index("if (widget == NULL") < body.index("text_value")
print("Badge Redisplay validates its widget")
