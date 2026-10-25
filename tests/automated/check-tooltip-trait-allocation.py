#!/usr/bin/env python3
"""Guard XmSetToolTipString against trait allocation failure."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ToolTip.c").read_text(
    encoding="utf-8"
)
start = text.index("XmSetToolTipString")
body = text[start:text.index("\n}\n", start) + 2]
allocation = "ttp = (XmToolTipTrait)XtCalloc(1, sizeof(XmToolTipTraitRec));"
if allocation not in body:
    raise SystemExit("tooltip trait allocation not found")
if "if (!ttp)" not in body or "return;" not in body:
    raise SystemExit("tooltip trait allocation failure is not handled")
if body.index("if (!ttp)") > body.index("XmeTraitSet"):
    raise SystemExit("trait registration precedes allocation failure check")

print("Tooltip trait allocation checks passed")
