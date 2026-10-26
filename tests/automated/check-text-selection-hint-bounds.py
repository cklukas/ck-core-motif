#!/usr/bin/env python3
"""Ensure primary and secondary selection hints saturate button coordinates."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
for function, next_function in (
    ("SetSelectionHint", "StartPrimary"),
    ("StartSecondary", "StartDrag"),
):
    start = source.rindex(f"\n{function}(")
    start = source.rfind("static ", 0, start)
    end = source.index(f"\nstatic void \n{next_function}", start)
    region = source[start:end]
    assert "ClampTextPointCoordinate(event->xbutton.x)" in region, function
    assert "ClampTextPointCoordinate(event->xbutton.y)" in region, function

print("Text selection hints: button coordinates are saturated")
