#!/usr/bin/env python3
"""Guard Outline argument and hierarchy child counters."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Outline.c").read_text(
    encoding="utf-8"
)
constraint_start = text.index("ConstraintSetValues(", 20000)
height_start = text.index("GetNodeHeightAndWidth(", 30000)
constraint = text[constraint_start:text.index("\n}\n", constraint_start) + 2]
height = text[height_start:text.index("\n}\n", height_start) + 2]
if "*num_args" not in constraint or "XmHierarchyC_num_children" not in height:
    raise SystemExit("Outline traversals are missing")
if "int i;" in constraint or "register int i;" in height:
    raise SystemExit("Outline retains a signed traversal counter")
if "Cardinal i;" not in constraint or "register Cardinal i;" not in height:
    raise SystemExit("Outline is missing Cardinal traversal counters")

print("Outline count bounds checks passed")
