#!/usr/bin/env python3
"""Check XPM scanning falls back for uncommon ZPixmap widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text()
start = source.index("if (image) {", source.index("scan the image data"))
end = source.index("if (ErrorStatus != XpmSuccess)", start)
dispatch = source[start:end]
assert "image->format == ZPixmap" in dispatch
assert "else\n\t\tErrorStatus = GetImagePixels(image, width, height, &pmap);" in dispatch
print("PASS XPM ZPixmap fallback dispatch")
