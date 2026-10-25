#!/usr/bin/env python3
"""Check tab-list copy rolls back later tab allocation failures."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabListCopy(XmTabList")
end = text.index("/* \n * Marked tabs", start)
block = text[start:end]

if "if (next_tab == NULL)" not in block:
    raise SystemExit("tab-list copy has no later-tab failure guard")
if "_XmTabLCount(tl) = i;" not in block:
    raise SystemExit("tab-list copy does not record the partial count")
if "XmTabListFree(tl);" not in block:
    raise SystemExit("tab-list copy does not release partial output")

print("tab-list copying rolls back partial allocation failures")
