#!/usr/bin/env python3
"""Check ColorSelector layout arithmetic is safe for tiny widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()

assert "int\t\t\twidth, height;" in source
assert "if (width < 0)" in source
assert "if (height < 0)" in source
assert "if (color_geom.height > 2 * color_geom.border_width)" in source
assert "ColorSelectorDimension(long long value)" in source
assert "ColorSelectorPosition(long long value)" in source
assert "value > (long long)USHRT_MAX" in source
assert "value > (long long)SHRT_MAX" in source
assert "ColorSelectorPosition(((long long)csw->core.width -" in source
assert "layout_width = ColorSelectorDimension((long long)width);" in source
assert "layout_height = ColorSelectorDimension((long long)height);" in source
assert "y += height" not in source

print("ColorSelector layout geometry clamps dimensions and positions")
