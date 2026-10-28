#!/usr/bin/env python3
"""Ensure DataGrid event dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start : source.index("\nstatic XtGeometryResult QueryGeometry", start)]
guard = "if (widget == NULL || event == NULL || !XmIsDataGrid(widget)) return;"
assert guard in body
assert body.index(guard) < body.index("event->type")
print("PASS DataGrid event callbacks validate widget and event inputs")
