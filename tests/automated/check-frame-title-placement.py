#!/usr/bin/env python3
"""Check Frame title placement avoids unsigned coordinate wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Frame.c").read_text()

assert "int child_x;" in source
assert "(int) fw->core.width - (int) spacing" in source
assert "(int) fw->core.width / 2 - (int) childWidth / 2" in source
assert "childX = (Position) child_x;" in source

print("Frame title placement arithmetic is signed")
