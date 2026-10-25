#!/usr/bin/env python3
"""Check that ColorObj pixel data validates its screen index."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nFetchPixelData")
end = source.index("static void", start + 1)
region = source[start:end]
assert "screen < 0" in region
assert "screen >= tmpColorObj->color_obj.numScreens" in region
assert "colorUse == NULL" in region
assert "w == NULL" in region
print("PASS ColorObj pixel data validates screen index")
