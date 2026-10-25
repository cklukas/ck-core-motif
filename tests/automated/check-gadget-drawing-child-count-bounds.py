#!/usr/bin/env python3
"""Guard gadget and DrawingArea child traversals against signed/empty-list bugs."""

from pathlib import Path

gadget = (Path(__file__).parents[2] / "lib" / "Xm" / "GadgetUtil.c").read_text(
    encoding="utf-8"
)
drawing = (Path(__file__).parents[2] / "lib" / "Xm" / "DrawingA.c").read_text(
    encoding="utf-8"
)
if "register int i;" in gadget or "register int i;" in drawing:
    raise SystemExit("gadget/DrawingArea traversal retains signed index")
if "register Cardinal i;" not in gadget or "register Cardinal i;" not in drawing:
    raise SystemExit("gadget/DrawingArea traversal is missing Cardinal index")
if "for (i = cw->composite.num_children; i > 0; )" not in drawing:
    raise SystemExit("DrawingArea reverse traversal is not empty-list safe")

print("Gadget/DrawingArea child-count bounds checks passed")
