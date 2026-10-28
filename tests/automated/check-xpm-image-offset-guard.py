#!/usr/bin/env python3
"""Check XPM image serialization guards width/cpp multiplication."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("now we know the size needed")
region = source[start : source.index("if (image->height > UINT_MAX", start)]

assert "image->cpp == 0" in region
assert "(UINT_MAX - 1) / image->cpp" in region
assert region.index("image->cpp == 0") < region.index("offset = image->width * image->cpp + 1")
print("XPM image serialization guards width/cpp multiplication")
