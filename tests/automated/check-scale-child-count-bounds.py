#!/usr/bin/env python3
"""Guard XmScale child-array traversals against signed index regressions."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(
    encoding="utf-8"
)

for function in ("MaxLabelWidth", "MaxLabelHeight", "LayoutHorizontalLabels", "LayoutVerticalLabels"):
    start = text.index("\n" + function + "(\n", 50000) + 1
    next_start = len(text)
    for candidate in ("\nstatic ", "\n#ifdef ", "\n#endif"):
        pos = text.find(candidate, start + len(function) + 1)
        if pos != -1:
            next_start = min(next_start, pos)
    body = text[start:next_start]
    if "composite.num_children" not in body:
        raise SystemExit(f"{function} no longer traverses child count")
    if "int i;" in body or "register int i;" in body:
        raise SystemExit(f"{function} uses a signed child index")

print("Scale child-count bounds checks passed")
