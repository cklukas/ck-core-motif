#!/usr/bin/env python3
"""Check defensive input handling in MWM frame-part geometry."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")
start = source.index(
    "Boolean GetFramePartInfo (ClientData *pcd, int part, int *pX, int *pY,"
)
end = source.index("} /* END OF FUNCTION", start)
body = source[start:end]

guard = (
    "if (pcd == NULL || pX == NULL || pY == NULL ||\n"
    "\t pWidth == NULL || pHeight == NULL)"
)
assert guard in body
assert "return (FALSE);" in body
assert body.index(guard) < body.index("TitleBarHeight (pcd)")
assert body.index(guard) < body.index("pcd->decor")

print("PASS MWM frame-part geometry rejects null inputs")
