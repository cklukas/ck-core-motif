#!/usr/bin/env python3
"""Check drag-under animation state allocation and caller handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
if source.count("if (aSaveData == NULL)") < 2:
    raise SystemExit("drag-under save-data allocation is not guarded at both sites")
if "aSaveData = CreateAnimationSaveData" not in source:
    raise SystemExit("drag-under caller is missing")

print("DragUnder animation-allocation checks passed")
