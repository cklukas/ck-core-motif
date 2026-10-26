#!/usr/bin/env python3
"""Check SaveSegments validates and cleans up each scratch pixmap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("SaveSegments(", source.index("static Boolean", source.index("SaveSegments(") + 1))
end = source.index("DrawHighlight", start)
context = source[start:end]

if context.count("pData->pixmap == None") != 4:
    raise SystemExit("SaveSegments must check all four scratch pixmaps")
if context.count("aSaveData->numSavedPixmaps++") != 4:
    raise SystemExit("SaveSegments must count only successfully allocated pixmaps")
if context.count("FreeSavedPixmaps (aSaveData);") != 4:
    raise SystemExit("SaveSegments must release partial backing stores on failure")

print("DragUnder save-segment checks passed")
