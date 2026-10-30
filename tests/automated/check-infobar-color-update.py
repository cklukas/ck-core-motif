#!/usr/bin/env python3
"""Check InfoBar live color changes update its drawing GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
assert "Boolean colors_changed=" in body
assert "XSetForeground(XtDisplay(widget),bar->info.gc" in body
assert "XSetBackground(XtDisplay(widget),bar->info.gc" in body
assert "if(colors_changed&&bar->info.gc)" in body

print("InfoBar color changes update the live drawing GC")
