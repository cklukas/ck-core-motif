#!/usr/bin/env python3
"""Check DrawHighlight validates its graphics-context allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("DrawHighlight(\n        XmAnimationSaveData")
end = source.index("DrawShadow", start)
context = source[start:end]

required = (
    "aSaveData->highlightGC =",
    "if (aSaveData->highlightGC == NULL)",
    "return;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DrawHighlight GC guard missing: " + ", ".join(missing))

print("DragUnder highlight-GC check passed")
