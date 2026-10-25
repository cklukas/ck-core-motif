#!/usr/bin/env python3
"""Check Container drag-icon pixmap metadata outputs are initialized."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (pixmap != XmUNSPECIFIED_PIXMAP)")
end = text.index("XtSetArg(wargs[n],XmNdragOperations", start)
body = text[start:end]
if "unsigned int rh = 0, rw = 0;" not in body:
    raise SystemExit("Container drag-icon dimensions are not initialized")
if "int rd = 0;" not in body:
    raise SystemExit("Container drag-icon depth is not initialized")
if "XmeGetPixmapData(XtScreen(cw), pixmap" not in body:
    raise SystemExit("Container drag-icon pixmap lookup is missing")
print("Container drag-icon pixmap metadata outputs are initialized")
