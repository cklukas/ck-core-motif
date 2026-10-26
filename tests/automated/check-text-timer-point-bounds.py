#!/usr/bin/env python3
"""Ensure timer-scrolling motion coordinates cannot narrow by wrapping."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \nCheckTimerScrolling(")
end = source.index("\n/* ARGSUSED */\nstatic void\nStartExtendSelection", start)
region = source[start:end]

assert "data->select_pos_x = ClampTextPointCoordinate(event->xmotion.x);" in region
assert "data->select_pos_y = ClampTextPointCoordinate(event->xmotion.y);" in region
assert "data->select_pos_x = event->xmotion.x;" not in region
assert "data->select_pos_y = event->xmotion.y;" not in region

print("Text timer scrolling: motion coordinates are saturated")
