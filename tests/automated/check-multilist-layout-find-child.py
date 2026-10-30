#!/usr/bin/env python3
"""Check MultiList layout does not dereference a missing find-text child."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text()
start = source.index("static void LayoutChildren")
end = source.index("static void PositionFindAndFindText", start)
body = source[start:end]
assert "XmMultiList_show_find(elist) && find_text != NULL" in body
assert "find_text->core.y" in body
assert body.index("find_text != NULL") < body.index("find_text->core.y")

print("MultiList layout guards the optional find-text child")
