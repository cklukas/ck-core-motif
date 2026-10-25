#!/usr/bin/env python3
"""Guard XmForm child-array traversals against signed index regressions."""

from pathlib import Path
import re

source = Path(__file__).parents[2] / "lib" / "Xm" / "Form.c"
text = source.read_text(encoding="utf-8")

start = text.index("QueryGeometry(", 30000)
change = text.index("ChangeManaged(", 40000)
sort_start = text.index("SortChildren(", 50000)
regions = {
    "QueryGeometry": text[start:change],
    "ChangeManaged": text[change:sort_start],
    "SortChildren": text[sort_start:],
}
for function, body in regions.items():
    if "composite.num_children" not in body:
        raise SystemExit(f"{function} no longer traverses child count")

if "\t\tint i;" in regions["QueryGeometry"]:
    raise SystemExit("QueryGeometry uses a signed child index")
if "    int i, j, k;" in regions["ChangeManaged"]:
    raise SystemExit("ChangeManaged uses signed child indices")
if "\tint i, j;" in regions["SortChildren"]:
    raise SystemExit("SortChildren uses signed child indices")

if "Cardinal sortedCount" not in regions["SortChildren"]:
    raise SystemExit("SortChildren must count children with Cardinal")

print("Form child-count bounds checks passed")
