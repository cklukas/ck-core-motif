#!/usr/bin/env python3
"""Check DataGrid preserves its render table when copying a replacement fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataGrid.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static void draw_text", start)
context = source[start:end]
required = (
    "if (replacement)",
    "grid->data_grid.render_table = old->data_grid.render_table;",
    "render_changed = False;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DataGrid render-table rollback missing: " + ", ".join(missing))

print("DataGrid render-table rollback check passed")
