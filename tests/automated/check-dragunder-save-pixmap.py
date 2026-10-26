#!/usr/bin/env python3
"""Check SaveAll validates its scratch pixmap before XCopyArea."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
start = source.index("pData->pixmap =", source.index("SaveAll("))
end = source.index("XCopyArea", start)
context = source[start:end]
required = (
    "pData->pixmap == None",
    "pData->pixmap == XmUNSPECIFIED_PIXMAP",
    "XtFree((char *)pData);",
    "aSaveData->numSavedPixmaps = 0;",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("SaveAll pixmap guard missing: " + ", ".join(missing))

print("DragUnder save-pixmap checks passed")
