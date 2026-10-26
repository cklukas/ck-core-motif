#!/usr/bin/env python3
"""Check that ScrollBar slider rendering tolerates pixmap creation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c"
text = source.read_text()
start = text.index("\nDrawSliderPixmap(")
end = text.index("\n}\n", start) + 3
block = text[start:end]

if "register Drawable slider = sbw->scrollBar.pixmap;" not in block:
    raise SystemExit("ScrollBar draw path does not use the cached slider drawable")
if "if (slider == None)" not in block:
    raise SystemExit("ScrollBar draw path does not guard a failed slider pixmap")

print("ScrollBar slider rendering guards failed pixmap creation")
