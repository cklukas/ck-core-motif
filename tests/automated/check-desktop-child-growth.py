#!/usr/bin/env python3
"""Guard XmDesktop child-list growth against overflow and realloc failure."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Desktop.c").read_text(
    encoding="utf-8"
)
start = text.index("InsertChild(")
body = text[start:text.index("\nstatic void \nDeleteChild", start)]
for needle in (
    "(Cardinal)(~(Cardinal)0 - 2)",
    "SIZE_MAX / sizeof(*new_children)",
    "if (new_children == NULL)",
    "cw->desktop.num_slots = new_slots",
):
    if needle not in body:
        raise SystemExit(f"missing Desktop growth guard: {needle}")
if "(unsigned) (cw->desktop.num_slots) * sizeof(Widget)" in body:
    raise SystemExit("Desktop growth retains narrowing allocation cast")

print("Desktop child-growth bounds checks passed")
