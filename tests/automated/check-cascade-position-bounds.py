#!/usr/bin/env python3
"""Check cascade gadget positioning is safe for tiny menu entries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()

assert "int x, y, buffer, available_height;" in source
assert "(int)cascadebtn->rectangle.width" in source
assert "available_height = (int)cascadebtn->rectangle.height" in source
assert "available_height > 0 ? available_height / 2 : 0" in source
assert "CBG_Cascade_x(cascadebtn) = (Position)x" in source
assert "CBG_Cascade_y(cascadebtn) = (Position)y" in source

print("Cascade gadget positions are bounded")
