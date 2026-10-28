#!/usr/bin/env python3
"""Check XPM color writing validates inputs and propagates status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
assert "LFUNC(WriteColors, int" in source
assert "ErrorStatus = WriteColors(file, image->colorTable, image->ncolors," in source
start = source.index("WriteColors(\n")
region = source[start : source.index("static int\nWritePixels", start)]
assert "file == NULL || (ncolors != 0 && colors == NULL)" in region
assert "colors->string == NULL" in region
assert "if (ferror(file))" in region
assert "return XpmSuccess;" in region
print("XPM color writing validates inputs and propagates status")
