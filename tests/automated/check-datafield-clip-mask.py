#!/usr/bin/env python3
"""Check clip-mask X resource creation before drawing."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("df_GetClipMask(")
start = source.index("clip_mask = XCreatePixmap", start)
end = source.index("XFillRectangle", start)
context = source[start:end]
required = (
    "if (clip_mask == None)",
    "if (fillGC == NULL)",
    "XFreePixmap(dpy, clip_mask);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("clip-mask resource guard missing: " + ", ".join(missing))

print("DataField clip-mask checks passed")
