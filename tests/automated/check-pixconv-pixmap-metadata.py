#!/usr/bin/env python3
"""Check PixConv guards GC setup on pixmap metadata lookup success."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "PixConv.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmGetPixmapBasedGC(")
end = text.index("\n\n\n", start)
body = text[start:end]

lookup = "if (XmeGetPixmapData(XtScreen(w), pixmap,"
if lookup not in body:
    raise SystemExit("PixConv metadata lookup is not guarded")
lookup_start = body.index(lookup)
if "&depth" not in body[lookup_start:body.index("))", lookup_start) + 2]:
    raise SystemExit("PixConv metadata lookup does not request depth")
guard_start = body.index("{", lookup_start)
if "if (depth == 1)" not in body[guard_start:]:
    raise SystemExit("PixConv depth handling is outside the success guard")
if "values.tile = pixmap" not in body[guard_start:]:
    raise SystemExit("PixConv tile setup is missing")
if body.count("XmeGetPixmapData(XtScreen(w), pixmap,") != 1:
    raise SystemExit("PixConv contains an unchecked duplicate metadata lookup")
print("PixConv pixmap metadata guard is present")
