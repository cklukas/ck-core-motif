#!/usr/bin/env python3
"""Check Drag ICC receiver resource fields honor their wire widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmWriteDSToStream(")
end = source.index("_XmFreeDragReceiverInfo(", start)
body = source[start:end]

assert "dsHeader.dsRegionNumBoxes = (CARD32)region->numRects;" in body
assert "animationPixmapDepth >" in body
assert "UINT16_MAX" in body
assert "(CARD32)info->animation_data.animationPixmap" in body
assert "(CARD32)info->animation_data.topShadowColor" in body

print("Drag ICC receiver resource widths are bounded")
