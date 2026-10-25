#!/usr/bin/env python3
"""Check IconG redisplay clamps vertical spans in signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "int available_height;" in source
assert "(int) ig->rectangle.height - 2 * (int) ht" in source
assert "(available_height > 0 ? available_height : 0)" in source
assert "(int) small_icon_y + (int) pm_height" in source
assert "clip_rect.height = (Dimension)" in source

print("IconG vertical render spans are bounded")
