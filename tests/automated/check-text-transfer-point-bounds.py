#!/usr/bin/env python3
"""Ensure selection-transfer point coordinates do not narrow by wrapping."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
assert "static Position ClampTextPointCoordinate(int coordinate);" in source
helper_start = source.index("static Position\nClampTextPointCoordinate(")
helper_end = source.index("\n\n/*", helper_start)
helper = source[helper_start:helper_end]
assert "coordinate < SHRT_MIN" in helper
assert "coordinate > SHRT_MAX" in helper
assert "return (Position)coordinate;" in helper

start = source.index("static void \nStuff(")
end = source.index("\n/* ARGSUSED */\nvoid \n_XmTextHandleSecondaryFinished", start)
region = source[start:end]
assert "point->x = ClampTextPointCoordinate(event->xbutton.x);" in region
assert "point->y = ClampTextPointCoordinate(event->xbutton.y);" in region

print("Text selection transfer: XPoint coordinates are saturated")
