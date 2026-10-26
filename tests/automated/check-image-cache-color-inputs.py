#!/usr/bin/env python3
"""Check that XPM image-cache color callbacks validate pointer inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
for signature, guard in (
    ("CacheColorPixel( Display", "display == NULL || xcolor == NULL"),
    ("GetCacheColor(Display", "display == NULL || xcolor == NULL"),
    ("FreeCacheColors(Display", "display == NULL || pixels == NULL || n <= 0"),
    ("ReleaseImagePixels(Screen", "screen == NULL || pixels == NULL || npixels <= 0"),
):
    start = source.index(signature, source.index("/*** COLOR CACHE NOW ***/"))
    body = source[start:source.index("\n}", start) + 2]
    assert guard in body
print("image-cache color and pixel callbacks validate pointer inputs")
