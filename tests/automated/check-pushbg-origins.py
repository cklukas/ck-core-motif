#!/usr/bin/env python3
"""Check PushBG draw origins use signed Position arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text()

assert source.count("(Position) ((int) pb->rectangle.x +") >= 8
assert source.count("(Position) ((int) pb->rectangle.y +") >= 10

print("PushBG draw origins are signed")
