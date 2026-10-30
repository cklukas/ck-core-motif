#!/usr/bin/env python3
"""Check DropHighlight handles temporary GC allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropHighlight.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void Redisplay(")
end = source.index("\nstatic Boolean SetValues", start)
region = source[start:end]

create = region.index("gc = XCreateGC")
guard = region.index("if (gc == NULL)", create)
draw = region.index("XDrawRectangle", create)
if guard > draw:
    raise SystemExit("DropHighlight must check temporary GC before drawing")
if "return;" not in region[guard : guard + 80]:
    raise SystemExit("DropHighlight missing-GC path must return")

print("PASS DropHighlight rendering handles missing graphics contexts")
