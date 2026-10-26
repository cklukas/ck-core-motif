#!/usr/bin/env python3
"""Check that image-cache hash cleanup callbacks validate their inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
clean_start = source.index("_XmCleanPixmapCache(Screen * screen")
clean_end = source.index("\n}", clean_start) + 2
assert "if (screen == NULL)" in source[clean_start:clean_end]
for name in ("CleanGCMapProc", "CleanPixmapMapProc"):
    start = source.index(name + " (XmHashKey")
    end = source.index("\n}", start) + 2
    region = source[start:end]
    assert "entry == NULL || ck == NULL || entry->screen == NULL" in region
print("image-cache cleanup callbacks validate entry and cleanup key")
