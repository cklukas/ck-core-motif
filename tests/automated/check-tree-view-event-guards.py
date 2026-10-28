#!/usr/bin/env python3
"""Ensure TreeView event dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start : source.index("\nstatic Boolean SetValues", start)]
guard = "if (widget == NULL || event == NULL || !XmIsTreeView(widget)) return;"
assert guard in body
assert body.index(guard) < body.index("event->type")
print("PASS TreeView event callbacks validate widget and event inputs")
