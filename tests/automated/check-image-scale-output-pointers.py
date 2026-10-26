#!/usr/bin/env python3
"""Check that image-scaling helpers validate output pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("XmRoundScaled(double value")
end = source.index("\n\n/***", start)
region = source[start:end]
assert "rounded == NULL" in region
assert "dimension == NULL" in region
print("image-scaling helpers validate output pointers")
