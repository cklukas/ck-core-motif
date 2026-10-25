#!/usr/bin/env python3
"""Check derived icon-cache directory allocation is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("dirName = (String)XtMalloc")
body = source[start:source.index("iNameEntry->dirName = dirName", start)]
assert "if (dirName == NULL)" in body
assert "XtFree(iNameEntry->key_name);" in body
assert "XtFree((char *)iNameEntry);" in body
assert "goto skip_icon_name_cache;" in body
print("PASS icon-file directory allocation is transactional")
