#!/usr/bin/env python3
"""Ensure Text drag-distance arithmetic cannot overflow int."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \ndragged(")
end = source.index("\n/* ARGSUSED */\nstatic void \nDoExtendedSelection", start)
region = source[start:end]

assert "int64_t xdiff, ydiff;" in region
assert "(int64_t)selectionHint.x - (int64_t)event->xbutton.x" in region
assert "(int64_t)selectionHint.y - (int64_t)event->xbutton.y" in region
assert "abs(selectionHint.x - event->xbutton.x)" not in region

print("Text drag distance: coordinate differences are widened before magnitude")
