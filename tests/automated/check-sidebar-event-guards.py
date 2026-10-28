#!/usr/bin/env python3
"""Ensure Sidebar keyboard dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void key_handler(Widget widget")
body = source[start : source.index("\nWidget XmCreateSidebar", start)]
guard = "if (widget == NULL || event == NULL || !XmIsSidebar(widget))"
assert guard in body
assert body.index(guard) < body.index("event->type")
print("PASS Sidebar keyboard callbacks validate widget and event inputs")
