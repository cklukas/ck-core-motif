#!/usr/bin/env python3
"""Check that XmRenderT rendition cloning handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
clone = source.index("CloneRendition(XmRendition rend)\n{")
renew = source.index("RenewRendition(XmRendition rend)\n{", clone)
clone_region = source[clone:renew]
renew_end = source.index("/* Allocate a new handle", renew)
renew_region = source[renew:renew_end]
assert "if (copy == NULL)" in clone_region
assert "if (copy_handle == NULL)" in clone_region
assert "XtFree((char *)copy);" in clone_region
assert "if (copy == NULL)" in renew_region
add_start = source.index("XmRenderTableAddRenditions(")
add_end = source.index("/* Remove matching renditions. */", add_start)
add_region = source[add_start:add_end]
assert "matches =" in add_region
assert "if (matches == NULL)" in add_region
assert "goto unlock_return;" in add_region
assert "if (table == NULL)" in add_region
assert "if (oldtable == NULL)" in add_region
assert "initial_copy_failed:" in add_region
assert "FreeRendition(_XmRTRenditions(oldtable)[j]);" in add_region
assert "copy = CopyRendition(rend);" in add_region
assert "copy = CloneRendition(previous);" in add_region
assert "copy = CloneRendition(rend);" in add_region
assert "if (copy == NULL)" in add_region
assert "if (newtable == NULL)" in add_region
assert "goto merge_abort;" in add_region
assert "while (i > 0)" in add_region
assert "source_table = oldtable;" in add_region
assert "append_copy_failed:" in add_region
assert "append_abort:" in add_region
assert "while (next > _XmRTCount(source_table))" in add_region
remove_start = source.index("_XmRenderTableRemoveRenditions(")
remove_end = source.index("static void", remove_start)
remove_region = source[remove_start:remove_end]
assert "if (table == NULL)" in remove_region
assert "if (newtable == NULL)" in remove_region
assert "while (i > 0)" in remove_region
assert "return(oldtable);" in remove_region
assert "if (table == NULL)" in remove_region
assert "if (newtable == NULL)" in remove_region
print("PASS XmRenderT rendition allocations are guarded")
