#!/usr/bin/env python3
"""Check CommandPalette keeps preferred and child geometry representable."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)

assert "static Dimension clamp_dimension(unsigned long value)" in source
assert "value > USHRT_MAX" in source
assert "static Position list_position(Dimension value)" in source
assert "value >= SHRT_MAX ? (Position)(SHRT_MAX - 1)" in source

start = source.index("static void natural")
end = source.index("static void layout", start)
natural = source[start:end]
assert "*height = clamp_dimension((unsigned long)sh + (lh ? lh : 120));" in natural

start = source.index("static void layout")
end = source.index("static void negotiate", start)
layout = source[start:end]
assert "list_y = list_position(sh);" in layout
assert "(unsigned long)list_height > (unsigned long)SHRT_MAX - (unsigned long)list_y" in layout
assert "if (!list_height) list_height = 1;" in layout

print("PASS CommandPalette geometry stays within Dimension and Position bounds")
