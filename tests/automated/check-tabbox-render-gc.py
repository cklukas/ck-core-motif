#!/usr/bin/env python3
"""Check TabBox redisplay handles drawing-GC allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
start = source.index("Redisplay(Widget widget")
end = source.index("\nstatic void\n#ifndef _NO_PROTO\nDrawTab", start)
region = source[start:end]

assign = region.index("XmTabBox__tab_GC(tab) = XmTabBox__text_GC(tab) = gc;")
guard = region.index("if (gc == NULL)", assign)
init = region.index("if( XmTabBox__inited(tab) == False )", guard)
if guard > init:
    raise SystemExit("TabBox GC failure guard must precede initialized drawing")
if "XDestroyRegion(damage)" not in region[guard:init]:
    raise SystemExit("TabBox GC failure path must release damage region")

print("PASS TabBox rendering handles missing graphics contexts")
