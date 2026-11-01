#!/usr/bin/env python3
"""Check temporary MWM icon-bevel GCs are released on every path."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmImage.c").read_text(
    encoding="utf-8"
)
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap */", start)
region = source[start:end]
failure = region[region.index("if (topGC == NULL || botGC == NULL)"):
                 region.index("top_rects->used = 0")]
success = region[region.index("XFillRectangles (DISPLAY, iconPixmap, botGC"):
                 region.rindex("return (iconPixmap)")]

assert failure.count("XtReleaseGC(pSD->screenTopLevelW") == 2
assert failure.index("XtReleaseGC") < failure.index("XFreePixmap")
assert success.count("XtReleaseGC(pSD->screenTopLevelW") == 2
print("PASS MWM icon bevel GCs are released after use and on partial failure")
