#!/usr/bin/env python3
"""Ensure negative-depth normalization cannot negate INT_MIN."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGetScaledPixmap(\n    Screen")
end = source.index("\n\n\n\n/************************************************************************", start)
region = source[start:end]

guard = "if (depth == INT_MIN)"
assert guard in region
assert region.index(guard) < region.index("if (depth < 0)")
assert "depth = -depth" in region

print("ImageCache rejects INT_MIN before negative-depth normalization")
