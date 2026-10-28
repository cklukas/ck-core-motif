#!/usr/bin/env python3
"""Check XPM data serialization validates exact color-token widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
colors_start = source.index("static int\nCreateColors(")
pixels_start = source.index("static int\nCreatePixels(", colors_start)
colors = source[colors_start:pixels_start]
pixels = source[pixels_start:source.index("static int\nCountExtensions", pixels_start)]
assert "if (cpp == 0 ||" in colors
assert "colors->string == NULL || strlen(colors->string) < cpp" in colors
assert "memcpy(buf, colors->string, cpp);" in colors
assert "colors[*pixels].string == NULL" in pixels
assert "strlen(colors[*pixels].string) < cpp" in pixels
assert pixels.count("memcpy(s, colors[*pixels].string, cpp);") == 2
assert "strncpy(s, colors[*pixels].string, cpp)" not in pixels
print("PASS XPM data serialization validates color token widths")
