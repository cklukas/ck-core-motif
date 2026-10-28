#!/usr/bin/env python3
"""Check XPM buffer serialization rejects zero chars-per-pixel safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
start = source.index("now we know the exact size we need")
region = source[start : source.index("p = (char *) XpmRealloc", start)]

assert "image->cpp == 0" in region
assert "UINT_MAX / image->cpp" in region
assert region.index("image->cpp == 0") < region.index("UINT_MAX / image->cpp")
print("XPM buffer serialization guards zero chars-per-pixel before division")
