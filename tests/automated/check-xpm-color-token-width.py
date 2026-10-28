#!/usr/bin/env python3
"""Check XPM color-table writing enforces the declared cpp width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
assert "LFUNC(WriteColors, int, (FILE *file, XpmColor *colors, unsigned int ncolors," in source
assert "ErrorStatus = WriteColors(file, image->colorTable, image->ncolors," in source
start = source.index("WriteColors(\n")
region = source[start : source.index("static int\nWritePixels", start)]
assert "unsigned int\t cpp" in region
assert "strlen(colors->string) < cpp" in region
print("XPM color-table writing enforces the declared cpp width")
