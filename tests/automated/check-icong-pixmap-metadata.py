#!/usr/bin/env python3
"""Check IconG pixmap metadata failures cannot select undefined geometry or GC data."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "IconG.c"
text = source.read_text(encoding="utf-8")
redisplay = text[text.index("static\tvoid\nRedisplay("):text.index("/************************************************************************\n * GetBaselines", text.index("static\tvoid\nRedisplay("))]
if redisplay.count("depth = 0;\n\tXmeGetPixmapData(XtScreen(wid)") != 2:
    raise SystemExit("IconG redisplay does not initialize both pixmap depths")
if "else \n\t    if (depth == 1)" not in redisplay:
    raise SystemExit("IconG redisplay plane fallback changed unexpectedly")

initialize = text[text.index("/** background gc **/"):text.index("IG_BackgroundGC(wid)", text.index("/** background gc **/"))]
if "depth = 0;" not in initialize or "else if (depth > 1)" not in initialize:
    raise SystemExit("IconG background GC does not ignore unresolved pixmap depth")

set_values = text[text.index("static\tBoolean\nSetValues("):text.index("if (IG_Detail(nw)", text.index("static\tBoolean\nSetValues("))]
if set_values.count("w = h = 0;") < 2:
    raise SystemExit("IconG SetValues does not reset both pixmap size lookups")
print("IconG pixmap metadata guards and size resets are present")
