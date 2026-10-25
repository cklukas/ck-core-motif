#!/usr/bin/env python3
"""Check Divider draw and label ownership failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Divider.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void Redisplay(Widget widget")
redisplay = text[start:text.index("static Boolean SetValues", start)]
assert "!window || !divider->divider.foreground_gc" in redisplay
start = text.index("static Boolean SetValues(Widget old_widget")
body = text[start:text.index("static XtGeometryResult QueryGeometry", start)]
assert "XmString replacement;" in body
assert "XmStringFree(old->divider.label);" in body
assert "divider->divider.label = old->divider.label;" in body
print("Divider drawing and label ownership are guarded")
