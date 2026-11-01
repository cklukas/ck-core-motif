#!/usr/bin/env python3
"""Ensure Sidebar keyboard dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void key_handler(Widget widget")
body = source[start : source.index("\nWidget XmCreateSidebar", start)]
for guard in ("widget == NULL", "event == NULL", "client == NULL", "!XmIsSidebar((Widget)client)"):
    assert guard in body
assert body.index("widget == NULL") < body.index("event->type")
print("PASS Sidebar keyboard callbacks validate widget and event inputs")
