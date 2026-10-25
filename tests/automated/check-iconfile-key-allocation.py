#!/usr/bin/env python3
"""Check icon-file cache key copying failure skips caching."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("iNameEntry->key_name = XtNewString")
body = source[start:source.index("#ifndef XTHREADS", start)]
assert "if (iNameEntry->key_name == NULL)" in body
assert "XtFree((char *)iNameEntry);" in body
assert "goto skip_icon_name_cache;" in body
print("PASS icon-file cache key allocation is guarded")
