#!/usr/bin/env python3
"""Check IconG clip rectangles clamp collapsed horizontal space."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "int available_width;" in source
assert "if (available_width < 0)" in source
assert "clip_rect.width = (Dimension) available_width;" in source
assert "clip_x = (int) ig->rectangle.x + (int) ig->rectangle.width" in source
assert "available_width = (int) next_tab_x - (int) detail_x;" in source

print("IconG clip widths are bounded")
