#!/usr/bin/env python3
"""Check image-cache callbacks make intentionally unused state explicit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
for name in ("UninstallImageMapProc", "CleanGCMapProc", "CleanPixmapMapProc"):
    start = source.rindex(f"{name} (XmHashKey")
    body = source[start : source.index("\n}", start) + 2]
    assert "(void)key;" in body
start = source.rindex("GetCacheColor(Display *")
body = source[start : source.index("\n}", start) + 2]
assert "(void)closure;" in body
print("PASS image-cache callback warning state is explicit")
