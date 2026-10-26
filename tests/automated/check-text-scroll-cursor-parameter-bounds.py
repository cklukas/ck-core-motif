#!/usr/bin/env python3
"""Ensure ScrollCursorVertically validates percentage and coordinates."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nScrollCursorVertically(")
end = source.index("\nstatic void \nAddNewLine", start)
region = source[start:end]

assert "ClampTextPointCoordinate(event->xbutton.x)" in region
assert "ClampTextPointCoordinate(event->xbutton.y)" in region
assert "sscanf(*params, \"%d\", &percentage) == 1" in region
assert "if (percentage < 0)" in region
assert "else if (percentage > 100)" in region
assert "if (data->number_lines == 0)" in region
assert "max_line / 100U" in region
assert "((max_line % 100U) * (LineNum)percentage) / 100U" in region

print("Text ScrollCursorVertically: percentage and point inputs are bounded")
