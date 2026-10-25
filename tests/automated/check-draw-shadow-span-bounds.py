#!/usr/bin/env python3
"""Check etched shared shadow drawing bounds its inner rectangle."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "int inner_width, inner_height, inner_thickness;" in source
assert "inner_width = (int) width - 2 * inner_thickness;" in source
assert "inner_height = (int) height - 2 * inner_thickness;" in source
assert "if (inner_width > 0 && inner_height > 0)" in source
assert "width - (shad_thick/2)*2" not in source
assert "height - (shad_thick/2)*2" not in source

print("etched shared shadow spans are bounded")
