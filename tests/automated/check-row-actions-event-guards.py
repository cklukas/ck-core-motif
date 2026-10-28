#!/usr/bin/env python3
"""Ensure RowActions event dispatch validates widget and event inputs first."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(
    encoding="utf-8"
)
start = source.index("static void events(Widget widget")
body = source[start : source.index("\nWidget XmCreateRowActions", start)]
guard = "if (widget == NULL || event == NULL || !XmIsRowActions(widget)) return;"
assert guard in body
assert body.index(guard) < body.index("event->type")
print("PASS RowActions event callbacks validate widget and event inputs")
