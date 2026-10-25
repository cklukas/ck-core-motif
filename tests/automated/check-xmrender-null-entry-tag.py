#!/usr/bin/env python3
"""Check render-table scans tolerate a malformed null entry tag."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
find_start = source.index("_XmRenderTableFindRendition(")
find_end = source.index("\n\n/*", find_start)
find_region = source[find_start:find_end]
assert "_XmRendTag(rend) != NULL" in find_region

remove_start = source.index("_XmRenderTableRemoveRenditions(XmRenderTable")
remove_end = source.index("\n\n/*", remove_start)
remove_region = source[remove_start:remove_end]
assert "_XmRendTag(_XmRTRenditions(oldtable)[i]) != NULL" in remove_region
print("PASS render-table lookup and removal guard null entry tags")
