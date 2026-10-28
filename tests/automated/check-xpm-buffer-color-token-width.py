#!/usr/bin/env python3
"""Check XPM buffer serialization rejects short or absent color tokens."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
colors = source[source.index("static int\nWriteColors(") : source.index(
    "static int\nWritePixels(", source.index("static int\nWriteColors(")
)]
pixels = source[source.index("static int\nWritePixels(") : source.index(
    "static unsigned int\nExtensionsSize", source.index("static int\nWritePixels(")
)]
assert "if (cpp == 0)" in colors
assert "colors->string == NULL || strlen(colors->string) < cpp" in colors
assert "memcpy(s, colors->string, cpp);" in colors
assert "colors[*pixels].string == NULL" in pixels
assert "strlen(colors[*pixels].string) < cpp" in pixels
assert "memcpy(s, colors[*pixels].string, cpp);" in pixels
print("PASS XPM buffer serialization validates color token widths")
