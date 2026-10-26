#!/usr/bin/env python3
"""Check that Container drag-icon X resources are validated before drawing."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()
start = text.index("dragIconInfo = (DragIconInfo)")
end = text.index("\n    n = 0;", start)
block = text[start:end]

for needle in (
    "if (cross == None)",
    "if (cross_m == None)",
    "if (tempgc == NULL)",
    "if (dragIconInfo -> state == NULL)",
    "XFreePixmap(XtDisplay(cw), cross)",
):
    if needle not in block:
        raise SystemExit(f"Container drag-icon setup is missing {needle}")

if block.index("XSaveContext") < block.index("dragIconInfo -> state == NULL"):
    raise SystemExit("drag-icon context is published before resource validation")

print("Container drag-icon resource creation is checked before drawing")
