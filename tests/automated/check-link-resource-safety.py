#!/usr/bin/env python3
"""Check Link draw and resource ownership failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Link.c"
text = source.read_text(encoding="utf-8")

start = text.index("static void Redisplay(Widget widget")
redisplay = text[start:text.index("static Boolean SetValues", start)]
assert "!XtWindow(widget) || !link->link.gc" in redisplay
start = text.index("static Boolean SetValues(Widget old_widget")
body = text[start:text.index("static XtGeometryResult QueryGeometry", start)]
assert "XmString label_replacement;" in body
assert "char *target_replacement;" in body
assert "XmStringFree(old->link.label);" in body
assert "if (target_replacement)" in body
assert "link->link.target = old->link.target;" in body
print("Link drawing and resource ownership are guarded")
