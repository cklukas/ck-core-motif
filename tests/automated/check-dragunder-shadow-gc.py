#!/usr/bin/env python3
"""Check DrawShadow and cleanup tolerate partial GC allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("DrawShadow(\n        XmAnimationSaveData")
end = source.index("DrawPixmap", start)
context = source[start:end]

required = (
    "if (aSaveData->topShadowGC == NULL)",
    "if (aSaveData->bottomShadowGC == NULL)",
    "XFreeGC (aSaveData->display, aSaveData->topShadowGC);",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DrawShadow GC guards missing: " + ", ".join(missing))

cleanup_start = source.index("FreeAnimationData(\n        XmAnimationSaveData")
cleanup_end = source.index("SaveAll", cleanup_start)
cleanup = source[cleanup_start:cleanup_end]
if cleanup.count("!= NULL") < 5:
    raise SystemExit("GC cleanup must guard all optional contexts")

print("DragUnder shadow-GC checks passed")
