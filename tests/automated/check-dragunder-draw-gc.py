#!/usr/bin/env python3
"""Check DragUnder initialization rejects a missing shared drawing GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("CreateAnimationSaveData(\n        XmDragContext")
end = source.index("FreeSavedPixmaps", start)
context = source[start:end]

required = (
    "aSaveData->drawGC =",
    "if (aSaveData->drawGC == NULL)",
    "XtFree ((char *)aSaveData);",
    "return NULL;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DragUnder draw-GC guard missing: " + ", ".join(missing))

print("DragUnder draw-GC check passed")
