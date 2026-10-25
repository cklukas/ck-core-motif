#!/usr/bin/env python3
"""Check tab-list replacement preserves the source on copy failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c"
text = source.read_text()
start = text.index("XmTabList\nXmTabListReplacePositions")
end = text.index("/*\n * This routine uses a mark/sweep algorithm", start)
block = text[start:end]

if "XmTabListCopy(oldlist, 0, 0)" not in block:
    raise SystemExit("replacement does not operate on a copied list")
if "if (tab == NULL)" not in block:
    raise SystemExit("replacement has no replacement-tab failure guard")
if "XmTabListFree(tl);" not in block:
    raise SystemExit("replacement does not release a partial copy")
if "return(oldlist);" not in block:
    raise SystemExit("replacement does not preserve the source on failure")

print("tab-list replacement rolls back allocation failures")
