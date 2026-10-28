#!/usr/bin/env python3
"""Ensure RatingControl event dispatch validates its inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events(Widget widget")
body = source[start:source.index("\nstatic Boolean SetValues", start)]
assert "rating==NULL||event==NULL||!XmIsRatingControl(widget)" in body
print("RatingControl event callbacks validate widget and event inputs")
