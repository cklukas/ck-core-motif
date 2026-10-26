#!/usr/bin/env python3
"""Check empty tab lists are released without traversing a null ring."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("TabListFreeUnlocked(XmTabList")
end = text.index("void\nXmTabListFree", start)
block = text[start:end]

if "_XmTabLCount(tablist) == 0" not in block:
    raise SystemExit("empty-list count guard is missing")
if "_XmTabLStart(tablist) == NULL" not in block:
    raise SystemExit("empty-list start guard is missing")
if "XtFree((char *)tablist);" not in block:
    raise SystemExit("empty-list cleanup does not free the list record")

print("empty tab lists are freed without null-ring traversal")
