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
create_start = source.index("_XmRenditionCreate(Display *display")
create_end = source.index("/* Mrm create function", create_start)
create_region = source[create_start:create_end]
assert "if (rend_int == NULL)" in create_region
assert "if (rend == NULL)" in create_region
assert "XtFree((char *)rend_int);" in create_region
table_start = source.index("_XmCreateRenderTable(Widget parent")
table_end = source.index("/* Mrm create function for renditions. */", table_start)
table_region = source[table_start:table_end]
assert "if (table == NULL)" in table_region
assert "if (newtable == NULL)" in table_region
assert "XtFree((char *)table);" in table_region
mrend_start = source.index("_XmCreateRendition(Widget parent")
mrend_end = source.index("/* Free data structure", mrend_start)
mrend_region = source[mrend_start:mrend_end]
assert "if (rt == NULL || GetPtr(rt) == NULL)" in mrend_region
assert "if (rend == NULL)" in mrend_region
assert "if (table == NULL)" in mrend_region
assert "copy = CopyRendition(rend);" in mrend_region
assert "if (copy == NULL)" in mrend_region
assert "required_count = (size_t)_XmRTCount(rt) + 1" in mrend_region
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in mrend_region
assert "XM_RESOURCE_PATH_CAPACITY - 3" in source
assert "names[XM_RESOURCE_PATH_CAPACITY]" in source
assert "classes[XM_RESOURCE_PATH_CAPACITY]" in source
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
assert "rendition_count > INT_MAX" in add_region
assert "SIZE_MAX / sizeof(Boolean)" in add_region
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in add_region
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
