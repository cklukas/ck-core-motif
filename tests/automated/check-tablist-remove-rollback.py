#!/usr/bin/env python3
"""Check tab-list removal preserves the source when copying fails."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabList\nXmTabListRemoveTabs")
end = text.index("XmTab\nXmTabCreate", start)
block = text[start:end]

if "tl = XmTabListCopy(oldlist, 0, 0);" not in block:
    raise SystemExit("removal does not prepare a private copy")
if "return(oldlist);" not in block:
    raise SystemExit("removal does not preserve the source on copy failure")
if "XmTabListFree(tl);" not in block:
    raise SystemExit("removal does not release the copied list")

print("tab-list removal rolls back copy allocation failures")
