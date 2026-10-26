#!/usr/bin/env python3
"""Check that XPM image-to-pixmap conversion validates X resources."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrPFrI.c"
text = source.read_text()
start = text.index("xpmCreatePixmapFromImage(")
end = text.index("\n}", start) + 2
block = text[start:end]

for needle in (
    "if (pixmap_return == NULL || ximage == NULL)",
    "if (*pixmap_return == None)",
    "if (gc == NULL)",
    "XFreePixmap(display, *pixmap_return);",
    "*pixmap_return = None;",
):
    if needle not in block:
        raise SystemExit(f"XPM pixmap conversion is missing {needle}")

print("XPM image-to-pixmap conversion validates resources")
