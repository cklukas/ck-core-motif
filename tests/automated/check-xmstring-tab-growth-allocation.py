#!/usr/bin/env python3
"""Check XmString tab-list growth handles tab allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
marker = "/* Need to add a tab */"
start = source.index(marker)
allocation = source.index("tab = XmTabCreate(0.0, units, offset_model", start)
guard = source.index("if (tab == NULL)", allocation)
link = source.index("_XmTabNext(prev) = tab;", guard)
assert allocation < guard < link
assert "XmTabListFree(tl);" in source[guard:link]
assert "return ((XmTabList)NULL);" in source[guard:link]
print("PASS XmString tab-list growth allocation failure is handled")
