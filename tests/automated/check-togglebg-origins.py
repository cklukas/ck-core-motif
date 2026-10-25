#!/usr/bin/env python3
"""Check ToggleBG draw origins use signed Position arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert source.count("(Position) ((int) w->rectangle.x +") >= 2
assert source.count("(Position) ((int) tb->rectangle.x +") >= 4
assert source.count("(Position) ((int) tb->rectangle.y +") >= 4

print("ToggleBG draw origins are signed")
