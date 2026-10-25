#!/usr/bin/env python3
"""Check icon-file cache leaf copying failure releases the entry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("iNameEntry->leafName = XtNewString(filePtr);")
body = source[start:source.index("        }", start)]
assert "if (iNameEntry->leafName == NULL)" in body
assert "XtFree(iNameEntry->dirName);" in body
assert "XtFree(iNameEntry->key_name);" in body
assert "goto skip_icon_name_cache;" in body
print("PASS icon-file leaf allocation is transactional")
