#!/usr/bin/env python3
"""Check tab-list conversion handles XmTabCreate failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToXmTabList(")
end = text.index("\ncvtStringToXmRenderTable(", start)
block = text[start:end]

if "tab = XmTabCreate" not in block:
    raise SystemExit("tab-list converter does not create tabs")
if "if (tab == NULL)" not in block:
    raise SystemExit("tab-list converter has no XmTabCreate guard")
if "XmTabListFree(tl);" not in block:
    raise SystemExit("tab-list converter does not release partial output")

print("tab-list conversion handles tab allocation failure")
