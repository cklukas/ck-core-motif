#!/usr/bin/env python3
"""Check DrawPixmap handles composite-mask resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("DrawPixmap(\n        XmAnimationSaveData")
end = source.index("AnimateExpose", start)
context = source[start:end]

required = (
    "mask != None && mask != XmUNSPECIFIED_PIXMAP",
    "if (maskGC != NULL)",
    "_XmFreeScratchPixmap (aSaveData->xmScreen, mask);",
    "if (mask == None || mask == XmUNSPECIFIED_PIXMAP)",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("DrawPixmap mask guards missing: " + ", ".join(missing))

print("DragUnder composite-mask checks passed")
