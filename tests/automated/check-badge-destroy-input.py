#!/usr/bin/env python3
"""Ensure Badge Destroy validates its widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Badge.c").read_text()
start = source.index("static void Destroy(Widget widget)")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass)" in body
assert body.index("if (widget == NULL") < body.index("badge->badge.foreground_gc")
print("Badge Destroy validates its widget")
