#!/usr/bin/env python3
"""Guard CareVisual composite traversal against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "CareVisualT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmNotifyChildrenVisual(")
body = text[start:]
if "composite.num_children" not in body:
    raise SystemExit("CareVisual notification no longer traverses child count")
if "register int i;" in body or "int i;" in body:
    raise SystemExit("CareVisual notification uses a signed child index")
if "register Cardinal i;" not in body:
    raise SystemExit("CareVisual notification is missing Cardinal index")

print("CareVisual child-count bounds checks passed")
