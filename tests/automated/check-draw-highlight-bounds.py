#!/usr/bin/env python3
"""Check shared highlight drawing clamps oversized thickness."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "int draw_width, draw_height, thickness;" in source
assert "if (thickness > draw_width)" in source
assert "if (thickness > draw_height)" in source
assert "draw_width - thickness" in source
assert "draw_height - thickness" in source
assert "x + width - highlight_thickness" not in source
assert "y + height - highlight_thickness" not in source

print("XmeDrawHighlight rectangles are bounded")
