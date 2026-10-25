#!/usr/bin/env python3
"""Check desktop color-cell insertion handles X11 color failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("XmeGetDesktopColorCells (Screen * screen")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "XColor icon_color;" in body
assert "!XParseColor" in body
assert "!XAllocColor" in body
assert "colors[i++] = icon_color;" in body
assert body.index("!XParseColor") < body.index("colors[i++] = icon_color;")

print("desktop color-cell allocation failures are handled")
