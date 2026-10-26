#!/usr/bin/env python3
"""Ensure horizontal page actions cannot wrap their target y-coordinate."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
left_start = source.index("static void \nMovePageLeft(")
left_end = source.index("\n/* ARGSUSED */\nstatic void \nMovePageRight", left_start)
right_start = source.index("static void \nMovePageRight(")
right_end = source.index("\n/* ARGSUSED */\nstatic void \nMovePageUp", right_start)

for region in (source[left_start:left_end], source[right_start:right_end]):
    assert "if (y > tw->text.output->data->font_ascent)" in region
    assert "y -= tw->text.output->data->font_ascent;" in region
    assert "y = 0;" in region

print("Text horizontal page actions: y movement is clamped")
