#!/usr/bin/env python3
"""Check CascadeBG shadow origins use signed Position arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()

assert source.count("(Position) ((int) cb->rectangle.x +") >= 2
assert source.count("(Position) ((int) cb->rectangle.y +") >= 2

print("CascadeBG shadow origins are signed")
