#!/usr/bin/env python3
"""Check I18List background fill height cannot underflow Dimension."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "if (height > ilist->primitive.shadow_thickness)" in source
assert "if (height > LINE_HEIGHT)" in source
assert "height -= ilist->primitive.shadow_thickness;\n\t    else" not in source
print("PASS I18List background height stays bounded")
