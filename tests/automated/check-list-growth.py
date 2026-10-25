#!/usr/bin/env python3
"""Check that List item and match-position growth handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

start = source.index("static Boolean\nAddItems(")
end = source.index("static void\nDeleteItems", start)
add_region = source[start:end]
assert "XmString *new_items;" in add_region
assert "nitems > INT_MAX - lw->list.itemCount" in add_region
assert "if (new_items == NULL)" in add_region
assert "(Cardinal)(sizeof(*new_items) *" in add_region
assert "return False;" in add_region
assert "if (!AddItems(lw, items, item_count, intern_pos))" in source

start = source.index("XmListGetMatchPos(")
end = source.index("\nvoid\nListScrollUp", start)
match_region = source[start:end]
assert "int *new_pos;" in match_region
assert "if (new_pos != NULL)" in match_region
assert "pos = new_pos;" in match_region

delete_start = source.index("static void\nDeleteItems")
delete_end = source.index("static void\nReplaceItem", delete_start)
delete_region = source[delete_start:delete_end]
assert "UINT_MAX / sizeof(XmString)" in delete_region
assert delete_region.count("(Cardinal)(sizeof(XmString) * (size_t)TotalItems)") >= 2

print("PASS List item and match-position growth checks allocation failures")
