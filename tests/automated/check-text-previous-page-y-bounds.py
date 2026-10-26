#!/usr/bin/env python3
"""Ensure MovePreviousPage cannot wrap its target y-coordinate."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nMovePreviousPage(")
end = source.index("\n/* ARGSUSED */\nstatic void \nMovePageLeft", start)
region = source[start:end]

assert "if (y > tw->text.output->data->font_ascent)" in region
assert "y -= tw->text.output->data->font_ascent;" in region
assert "y = 0;" in region

print("Text MovePreviousPage: top-edge y movement is clamped")
