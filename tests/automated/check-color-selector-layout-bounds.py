#!/usr/bin/env python3
"""Check ColorSelector layout arithmetic is safe for tiny widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()

assert "int\t\t\twidth, height;" in source
assert "if (width < 0)" in source
assert "if (height < 0)" in source
assert "if (color_geom.height > 2 * color_geom.border_width)" in source
assert "((int)csw->core.width - (int)radio_geom.width) / 2" in source

print("ColorSelector layout geometry is bounded")
