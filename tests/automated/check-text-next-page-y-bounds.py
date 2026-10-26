#!/usr/bin/env python3
"""Ensure MoveNextPage cannot wrap its target y-coordinate."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nMoveNextPage(")
end = source.index("\n/* ARGSUSED */\nstatic void \nMovePreviousPage", start)
region = source[start:end]

assert "y = ClampTextPointCoordinate((int)y + o_data->topmargin);" in region
assert "else if (y > tw->text.output->data->font_ascent)" in region
assert "y -= tw->text.output->data->font_ascent;" in region
assert "y = 0;" in region

print("Text MoveNextPage: top-edge y movement is clamped")
