#!/usr/bin/env python3
"""Check that image-cache GC creation validates screen and image inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("GetGCForPutImage(Screen *screen")
end = source.index("\n\n/************", start)
region = source[start:end]
assert "screen == NULL || image == NULL || pixmap == None" in region
assert region.index("screen == NULL") < region.index("DisplayOfScreen(screen)")
print("image-cache GC creation validates screen, image, and pixmap inputs")
