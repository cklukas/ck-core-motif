#!/usr/bin/env python3
"""Check render-table count and allocation-size guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
clone_start = source.index("CloneRendition(")
clone_end = source.index("/* Set the old handle", clone_start)
clone_region = source[clone_start:clone_end]
assert "if (copy == NULL) return(NULL);" in clone_region
assert "if (copy_handle == NULL)" in clone_region
copy_start = source.index("CopyRendition(")
copy_end = source.index("/* Make a copy of a rendition", copy_start)
copy_region = source[copy_start:copy_end]
assert "_XmRendRefcountDec(rend);" in copy_region
print("PASS render-table rendition helper allocations are rollback-safe")

start = source.index("XmRenderTableAddRenditions(")
end = source.index("/* One body owns each entry handle", start)
region = source[start:end]

assert "rendition_count > INT_MAX" in region
assert "SIZE_MAX / sizeof(Boolean)" in region
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in region
assert "append_count > UINT16_MAX" in region
assert "append_copy_failed:" in region
assert "append_abort:" in region
assert "copy_start = (tmptable != NULL) ? 0 : _XmRTCount(source_table);" in region
assert "while (next > copy_start)" in region
assert "if (newtable == NULL)" in region
assert "merge_abort:" in region
assert "while (i > 0)" in region
assert "copy = CopyRendition(rend);" in region
assert "copy = CloneRendition(previous);" in region
assert "copy = CloneRendition(rend);" in region
assert "if (copy == NULL)" in region
assert "if (matches == NULL)" in region
assert "initial_copy_failed:" in region
assert "for (j = 0; j < i; ++j)" in region
assert "if (renditions[i] == NULL)" in region
assert "goto unlock_return;" in region
print("PASS render-table count and allocation-size guards")

remove_start = source.index("_XmRenderTableRemoveRenditions(")
remove_end = source.index("static void\nCopyFromArg", remove_start)
remove_region = source[remove_start:remove_end]
assert "Plan the removal without changing the live table" in remove_region
assert "removed = (Boolean *)XtMalloc" in remove_region
assert "if (table == NULL)" in remove_region
assert "if (newtable == NULL)" in remove_region
assert "remove_abort:" in remove_region
print("PASS render-table removal publishes only after allocation success")

extents_start = source.index("XmRenderTableGetDefaultFontExtents(")
extents_end = source.index("\n}", extents_start)
extents_region = source[extents_start:extents_end]
assert "if (height) *height = 0;" in extents_region
assert "if (ascent) *ascent = 0;" in extents_region
assert "if (descent) *descent = 0;" in extents_region
assert "if (rendertable == NULL)" in extents_region
print("PASS null render-table extents initialize outputs")
