#!/usr/bin/env python3
"""Ensure Link event dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Link.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start : source.index("\nvoid XmLinkActivate", start)]
guard = "if (widget == NULL || event == NULL || !XmIsLink(widget)) return;"
assert guard in body
assert body.index(guard) < body.index("XtIsSensitive(widget)")
print("PASS Link event callbacks validate widget and event inputs")
