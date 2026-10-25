#!/usr/bin/env python3
"""Check Prompt GC and owned-text failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Prompt.c"
text = source.read_text(encoding="utf-8")

redisplay = text[text.index("static void Redisplay(Widget widget"):text.index("static Boolean SetValues(Widget old_widget")]
assert "!prompt->prompt.gc" in redisplay
start = text.index("static Boolean SetValues(Widget old_widget")
body = text[start:text.index("static XtGeometryResult QueryGeometry", start)]
assert "XmString replacement;" in body
assert "XmStringFree(old->prompt.text);" in body
assert "prompt->prompt.text = old->prompt.text;" in body
print("Prompt GC and text ownership are guarded")
