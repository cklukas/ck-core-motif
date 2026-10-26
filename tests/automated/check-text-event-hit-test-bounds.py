#!/usr/bin/env python3
"""Ensure Text event hit-tests saturate button coordinates."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
for function in (
    "SetCursorPosition",
    "StartPrimary",
    "InSelection",
    "ExtendSecondary",
    "MoveDestination",
):
    definition = source.rindex(f"\n{function}(")
    start = source.rfind("static ", 0, definition)
    next_static = source.find("\nstatic ", start + 1)
    next_public = source.find("\nvoid ", start + 1)
    ends = [value for value in (next_static, next_public) if value != -1]
    region = source[start:min(ends)] if ends else source[start:]
    assert "ClampTextPointCoordinate(event->xbutton.x)" in region, function
    assert "ClampTextPointCoordinate(event->xbutton.y)" in region, function

print("Text event hit-tests: button coordinates are saturated")
