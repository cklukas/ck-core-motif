#!/usr/bin/env python3
"""Check that Notebook spiral rendering validates pixmap creation."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c"
text = source.read_text()
needle = "pixmap = XCreatePixmap(XtDisplay(nb), XtWindow(nb), pw, ph, nb->core.depth);"
start = text.index(needle)
block = text[start:text.index("\n    /*", start)]

for required in (
    "if (pixmap == None)",
    "nb->notebook.spiral_pixmap = XmUNSPECIFIED_PIXMAP;",
    "return;",
    "nb->notebook.spiral_pixmap = pixmap;",
):
    if required not in block:
        raise SystemExit(f"Notebook spiral rendering is missing {required}")

print("Notebook spiral pixmap creation is guarded")
