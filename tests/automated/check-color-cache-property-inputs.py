#!/usr/bin/env python3
"""Check ColorObj cache-property input validation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nColorCachePropertyExists")
end = source.index("/**********************************************************************/\n/** GetSelection", start)
region = source[start:end]
assert "dpy == NULL" in region
assert "w == NULL" in region
assert "screen < 0" in region
assert "screen >= color_obj->color_obj.numScreens" in region
print("PASS ColorObj cache-property inputs are guarded")
