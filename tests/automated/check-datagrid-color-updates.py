#!/usr/bin/env python3
"""Check DataGrid refreshes its drawing GC for live color changes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataGrid.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static void draw_text", start)
context = source[start:end]
required = (
    "Boolean colors_changed =",
    "XSetForeground(XtDisplay(new_widget), grid->data_grid.gc,",
    "XSetBackground(XtDisplay(new_widget), grid->data_grid.gc,",
    "render_changed || colors_changed",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DataGrid color update missing: " + ", ".join(missing))

print("DataGrid color-update check passed")
