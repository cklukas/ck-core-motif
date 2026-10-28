#!/usr/bin/env python3
"""Check scaled-image destination coordinates stay in signed X11 range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
body = source[start:end]

assert "(long long)dest_x + (long long)dest_width > INT_MAX" in body
assert "dest_y > INT_MAX - (int)dest_height" in body

print("PASS scaled-image destination coordinates are bounded")
