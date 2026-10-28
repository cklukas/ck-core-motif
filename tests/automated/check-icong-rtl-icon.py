#!/usr/bin/env python3
"""Check exported IconG RTL icon positions use signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "(int) ig->rectangle.width - (int) large_icon_x -" in source
assert "(int) ig->rectangle.width - (int) small_icon_x -" in source
assert source.count("large_icon_x = bounded_position((long long)icon_x);") == 2
assert source.count("small_icon_x = bounded_position((long long)icon_x);") == 2

print("IconG RTL icon placement is signed")
