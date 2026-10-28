#!/usr/bin/env python3
"""Check XPM extension-line storage handles its initial allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmParseExtensions(")
body = source[start:source.index("int\nxpmParseData(", start)]
needle = "ext->lines = (char **) XpmMalloc(sizeof(char *));"
position = body.index(needle)
check = body[position : position + 180]
assert "if (ext->lines == NULL)" in check
assert "XpmFreeExtensions(exts, num + 1);" in check
assert "return (XpmNoMemory);" in check
print("PASS XPM extension lines handle initial allocation failure")
