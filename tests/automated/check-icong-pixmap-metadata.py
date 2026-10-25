#!/usr/bin/env python3
"""Check IconG pixmap metadata failures cannot select undefined geometry or GC data."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "IconG.c"
text = source.read_text(encoding="utf-8")
redisplay_start = text.index("static\tvoid\nRedisplay(")
redisplay = text[redisplay_start:text.index("/************************************************************************\n * GetBaselines", redisplay_start)]
if redisplay.count("depth = 0;\n\tXmeGetPixmapData(XtScreen(wid)") != 2:
    raise SystemExit("IconG redisplay does not initialize both pixmap depths")
if "else \n\t    if (depth == 1)" not in redisplay:
    raise SystemExit("IconG redisplay plane fallback changed unexpectedly")

background_start = text.index("/** background gc **/")
initialize = text[background_start:text.index("IG_BackgroundGC(wid)", background_start)]
if "depth = 0;" not in initialize or "else if (depth > 1)" not in initialize:
    raise SystemExit("IconG background GC does not ignore unresolved pixmap depth")

set_values_start = text.index("static\tBoolean\nSetValues(")
set_values = text[set_values_start:text.index("if (IG_Detail(nw)", set_values_start)]
if set_values.count("w = h = 0;") < 2:
    raise SystemExit("IconG SetValues does not reset both pixmap size lookups")
print("IconG pixmap metadata guards and size resets are present")
