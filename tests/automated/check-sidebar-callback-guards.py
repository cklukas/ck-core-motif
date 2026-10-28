#!/usr/bin/env python3
"""Ensure Sidebar action callbacks validate their closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void activate(Widget widget")
body = source[start : source.index("\nstatic void key_handler", start)]
guard = "!XmIsSidebar((Widget)sidebar)"
assert guard in body
assert body.index(guard) < body.index("XmSidebarToggle")
print("PASS Sidebar action callbacks validate closure type")
