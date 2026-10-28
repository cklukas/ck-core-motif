#!/usr/bin/env python3
"""Ensure RowActions activation callbacks validate their closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(
    encoding="utf-8"
)
start = source.index("static void activate(Widget button")
body = source[start : source.index("\nstatic void events", start)]
guard = "!XmIsRowActions((Widget)actions)"
assert guard in body
assert body.index(guard) < body.index("actions->actions.item_count")
print("PASS RowActions activation callbacks validate closure type")
