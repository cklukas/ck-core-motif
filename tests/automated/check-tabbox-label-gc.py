#!/usr/bin/env python3
"""Check TabBox label renderers require both bitmap graphics contexts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
needle = "if( XmTabBox__zero_GC(tab) == NULL || XmTabBox__one_GC(tab) == NULL )"
if source.count(needle) < 2:
    raise SystemExit("TabBox horizontal and vertical label paths need GC guards")

positions = [i for i in range(len(source)) if source.startswith(needle, i)]
if len(positions) < 2:
    raise SystemExit("TabBox horizontal and vertical label paths need GC guards")
for guard in positions[:2]:
    draw = source.find("XFillRectangle", guard)
    if draw < 0:
        raise SystemExit("TabBox GC guard is not followed by label drawing")

print("PASS TabBox label rendering handles missing bitmap graphics contexts")
