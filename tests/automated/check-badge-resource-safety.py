#!/usr/bin/env python3
"""Check Badge draw and label ownership failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Badge.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void Redisplay(Widget widget")
redisplay = text[start:text.index("static Boolean SetValues", start)]
assert "!XtWindow(widget) || !badge->badge.foreground_gc" in redisplay
start = text.index("static Boolean SetValues(Widget old_widget")
body = text[start:text.index("static XtGeometryResult QueryGeometry", start)]
assert "XmString replacement;" in body
assert "XmStringFree(old->badge.label);" in body
assert "badge->badge.label = old->badge.label;" in body
print("Badge drawing and label ownership are guarded")
