#!/usr/bin/env python3
"""Check that image-cache hash cleanup callbacks validate their inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
for name in ("CleanGCMapProc", "CleanPixmapMapProc"):
    start = source.index(name + " (XmHashKey")
    end = source.index("\n}", start) + 2
    region = source[start:end]
    assert "entry == NULL || ck == NULL" in region
print("image-cache cleanup callbacks validate entry and cleanup key")
