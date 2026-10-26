#!/usr/bin/env python3
"""Check XPM buffer serialization validates pixel indexes and spans."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nWritePixels")
body = source[start:source.index("static unsigned int\nExtensionsSize", start)]
assert "unsigned int\t ncolors" in body
assert "*pixels >= ncolors" in body
assert "(size_t)(s - dataptr) > data_size" in body
assert "return 0;" in body
call = source[source.index("if (!WritePixels"):source.index("RETURN(XpmFileInvalid);", source.index("if (!WritePixels"))]
assert "image->ncolors" in call
print("PASS XPM buffer serialization bounds pixel indexes")
