#!/usr/bin/env python3
"""Check IconG Redisplay keeps RTL icon origins in signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "int icon_x;" in source
assert "icon_x = (int) ig->rectangle.width -" in source
assert "(int) large_icon_x -" in source
assert "(int) small_icon_x -" in source
assert source.count("large_icon_x = bounded_position((long long)icon_x);") == 2
assert source.count("small_icon_x = bounded_position((long long)icon_x);") == 2

print("IconG RTL render placement is signed")
