#!/usr/bin/env python3
"""Check cached icon directory and leaf copies are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("iNameEntry->dirName = XtNewString(GdirName);")
body = source[start:source.index("#endif", start)]
assert "if (iNameEntry->dirName == NULL)" in body
assert "if (iNameEntry->leafName == NULL)" in body
assert body.count("XtFree(iNameEntry->key_name);") == 2
assert "goto skip_icon_name_cache;" in body
print("PASS cached icon path copies are transactional")
