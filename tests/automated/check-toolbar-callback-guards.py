#!/usr/bin/env python3
"""Ensure Toolbar action callbacks validate their closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void activate(Widget child")
body = source[start : source.index("\nWidget XmCreateToolbar", start)]
guard = "!XmIsToolbar((Widget)toolbar)"
assert guard in body
assert body.index(guard) < body.index("toolbar->toolbar.action_count")
print("PASS Toolbar action callbacks validate closure type")
