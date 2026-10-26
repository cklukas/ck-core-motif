#!/usr/bin/env python3
"""Check XPM file serialization validates pixel indexes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nWritePixels")
body = source[start:source.index("static void\nWriteExtensions", start)]
assert "unsigned int\tncolors" in body
assert "*pixels >= ncolors" in body
assert "colors[*pixels].string == NULL" in body
call = source[source.index("ErrorStatus = WritePixels"):source.index(";", source.index("ErrorStatus = WritePixels"))]
assert "image->ncolors" in call
print("PASS XPM file serialization bounds pixel indexes")
