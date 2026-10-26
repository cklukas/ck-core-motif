#!/usr/bin/env python3
"""Ensure RTL left-edge autoscroll updates the horizontal position."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \nCheckTimerScrolling(")
end = source.index("\n/* ARGSUSED */\nstatic void\nStartExtendSelection", start)
region = source[start:end]
marker = "/* left the text */"
left_branch = region[region.index(marker):]
assignment = "data->select_pos_x = ClampTextPointCoordinate((int)"
assert assignment in left_branch
assert "data->select_pos_y = ClampTextPointCoordinate((int)" not in left_branch.split(
    "/* ask the vertical scroller", 1
)[0]

print("Text RTL autoscroll: left-edge movement updates x")
