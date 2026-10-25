#!/usr/bin/env python3
"""Check that XmRenderT public rendition copy propagates allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("_XmRenditionCopy(XmRendition rend")
end = source.index("/* Creates new rendertable", start)
region = source[start:end]
assert "if (shared) toRend = CopyRendition(rend);" in region
assert "if (toRend == NULL)" in region
assert "return(NULL);" in region
assert "if (*toRend != *rend)" in region
print("PASS XmRenderT rendition-copy failure is propagated")

copy_start = source.index("XmRenderTableCopy(")
copy_end = source.index("/* One body owns each entry handle", copy_start)
copy_region = source[copy_start:copy_end]
assert "XmRenderTableGetRendition(table, tags[index])" not in copy_region
assert "_XmRenderTableFindRendition(table, tags[index]" in copy_region
assert "if (body == NULL)" in copy_region
assert "if (result == NULL)" in copy_region
assert "copy_failed:" in copy_region
assert "FreeRendition(_XmRTRenditions(result)[count]);" in copy_region
assert "FreeHandle(_XmRTRenditions(result)[count]);" in copy_region
assert "SIZE_MAX - sizeof(_XmRenderTableRec)" in copy_region
assert "capacity - RENDITIONS_IN_STRUCT" in copy_region
print("PASS XmRenderTableCopy avoids recursive lookup locking and cleans failures")
