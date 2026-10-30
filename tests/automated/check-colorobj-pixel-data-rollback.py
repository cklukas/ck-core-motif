#!/usr/bin/env python3
"""Ensure malformed color-object property data cannot publish partial state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()
start = source.index("static Boolean\nFetchPixelData")
end = source.index("static Boolean\nColorCachePropertyExists", start)
region = source[start:end]

parse = region.index("for (i = 0; i < XmCO_NUM_COLORS; i++)")
update = region.index("UpdateXrm (colors, screen, tmpColorObj);")
publish = region.index("tmpColorObj->color_obj.colorUse[screen] = colorUse;", update)

assert "tmpColorObj->color_obj.colorUse[screen] = colorUse;" not in region[:parse]
assert publish > update
assert "tmpColorObj->color_obj.colorIsRunning = True;" in region[publish:]
print("PASS ColorObj pixel data publishes selector only after complete validation")
